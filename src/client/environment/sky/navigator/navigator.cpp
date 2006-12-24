

#include "navigator.h"
#include "client/environment/sky/utils/time.h"
#include "client/environment/sky/utils/vecmath.h"


////////////////////////////////////////////////////////////////////////////////
Navigator::Navigator(Observator* obs) : flag_traking(0), flag_lock_equ_pos(0), flag_auto_move(0),
		time_speed(JD_SECOND), JDay(0.), position(obs)
{
	local_vision=csVector3(1.,0.,0.);
	equ_vision=csVector3(1.,0.,0.);
	prec_equ_vision=csVector3(1.,0.,0.);  // not correct yet...
	viewing_mode = VIEW_HORIZON;  // default
}

Navigator::~Navigator()
{
}

void Navigator::init()
{
	setJDay(get_julian_from_sys());
	setLocalVision(csVector3(1.0f,1.0f,0.2f));
	// Compute transform matrices between coordinates systems
	updateTransformMatrices();
	updateModelViewMat();
	setViewingMode(Navigator::VIEW_HORIZON);
	initViewPos = csVector3(1.0f,1.0f,0.2f);
	setLocalVision(initViewPos);
}

////////////////////////////////////////////////////////////////////////////////

void Navigator::updateVisionVector(int delta_time)
{
	
  if (flag_lock_equ_pos) // Equatorial vision vector locked
  {
    // Recalc local vision vector
    local_vision=earth_equ_to_local(equ_vision);
  }
  else // Local vision vector locked
  {
    // Recalc equatorial vision vector
    equ_vision=local_to_earth_equ(local_vision);
  }

  prec_equ_vision = mat_earth_equ_to_j2000*equ_vision;

}

////////////////////////////////////////////////////////////////////////////////
void Navigator::setLocalVision(csVector3 _pos)
{
	local_vision = _pos;
	equ_vision=local_to_earth_equ(local_vision);
	prec_equ_vision = mat_earth_equ_to_j2000*equ_vision;
}

////////////////////////////////////////////////////////////////////////////////
void Navigator::updateMove(double deltaAz, double deltaAlt)
{
	double azVision, altVision;
    azVision = 0;
    altVision = 0;

	if( viewing_mode == VIEW_EQUATOR) StelUtils::rect_to_sphe(&azVision,&altVision,equ_vision);
	else StelUtils::rect_to_sphe(&azVision,&altVision,local_vision);

	// if we are moving in the Azimuthal angle (left/right)
	if (deltaAz) azVision-=deltaAz;
	if (deltaAlt)
	{
		if (altVision+deltaAlt <= M_PI_2 && altVision+deltaAlt >= -M_PI_2) altVision+=deltaAlt;
		if (altVision+deltaAlt > M_PI_2) altVision = M_PI_2 - 0.000001;		// Prevent bug
		if (altVision+deltaAlt < -M_PI_2) altVision = -M_PI_2 + 0.000001;	// Prevent bug
	}

	// recalc all the position variables
	if (deltaAz || deltaAlt)
	{
		if( viewing_mode == VIEW_EQUATOR)
		{
			StelUtils::sphe_to_rect(azVision, altVision, equ_vision);
			local_vision=earth_equ_to_local(equ_vision);
		}
		else
		{
			StelUtils::sphe_to_rect(azVision, altVision, local_vision);
			// Calc the equatorial coordinate of the direction of vision wich was in Altazimuthal coordinate
			equ_vision=local_to_earth_equ(local_vision);
			prec_equ_vision = mat_earth_equ_to_j2000*equ_vision;
		}
	}

	// Update the final modelview matrices
	updateModelViewMat();

}

////////////////////////////////////////////////////////////////////////////////
// Increment time
void Navigator::updateTime(int delta_time)
{
	JDay+=time_speed*(double)delta_time/1000.;

	// Fix time limits to -100000 to +100000 to prevent bugs
	if (JDay>38245309.499988) JDay = 38245309.499988;
	if (JDay<-34803211.500012) JDay = -34803211.500012;
}

const csMatrix4 mat_j2000_to_vsop87(
              csMatrix4::xrotation(-23.4392803055555555556*(M_PI/180)) *
              csMatrix4::zrotation(0.0000275*(M_PI/180)));

const csMatrix4 mat_vsop87_to_j2000(mat_j2000_to_vsop87.GetTranspose());

////////////////////////////////////////////////////////////////////////////////

void Navigator::updateTransformMatrices(void)
{

  mat_local_to_earth_equ = position->getRotLocalToEquatorial(JDay);
  mat_earth_equ_to_local = mat_local_to_earth_equ.GetTranspose();

  mat_earth_equ_to_j2000 = (mat_vsop87_to_j2000 * position->getRotEquatorialToVsop87());
  mat_j2000_to_earth_equ = mat_earth_equ_to_j2000.GetTranspose();

  mat_helio_to_earth_equ = mat_j2000_to_earth_equ *
                           mat_vsop87_to_j2000 *
	                       csMatrix4::translation(-position->getCenterVsop87Pos());


	// These two next have to take into account the position of the observer on the earth
	csMatrix4 tmp =
	    mat_j2000_to_vsop87 *
	    mat_earth_equ_to_j2000 *
        mat_local_to_earth_equ;

	mat_local_to_helio =  csMatrix4::translation(position->getCenterVsop87Pos()) *
	                      tmp *
	                      csMatrix4::translation(csVector3(0.,0., position->getDistanceFromCenter()));

	mat_helio_to_local =  csMatrix4::translation(csVector3(0.,0.,-position->getDistanceFromCenter())) *
	                      tmp.GetTranspose() *
	                      csMatrix4::translation(-position->getCenterVsop87Pos());

}

////////////////////////////////////////////////////////////////////////////////
// Update the modelview matrices
void Navigator::updateModelViewMat(void)
{

  csVector3 f;

  if ( viewing_mode == VIEW_EQUATOR)
  {
    // view will use equatorial coordinates, so that north is always up
    f = equ_vision;
  }
  else
  {
    // view will correct for horizon (always down)
    f = local_vision;
  }


  f.Normalize();
  csVector3 s(f[1],-f[0],0.);


  if ( viewing_mode == VIEW_EQUATOR)
  {
    // convert everything back to local coord
    f = local_vision;
    f.Normalize();
    s = earth_equ_to_local( s );
  }

  csVector3 u(s%f);
  s.Normalize();
  u.Normalize();

  csMatrix4 matrix(s[0],u[0],-f[0],0.,
	               s[1],u[1],-f[1],0.,
	               s[2],u[2],-f[2],0.,
	               0.,0.,0.,1.);

  mat_local_to_eye = matrix;


  mat_earth_equ_to_eye = mat_local_to_eye*mat_earth_equ_to_local;
  mat_helio_to_eye = mat_local_to_eye*mat_helio_to_local;
  mat_j2000_to_eye = mat_earth_equ_to_eye*mat_j2000_to_earth_equ;
}

////////////////////////////////////////////////////////////////////////////////
// Return the observer heliocentric position
csVector3 Navigator::getObserverHelioPos(void) const
{
	csVector3 v(0.,0.,0.);
	return mat_local_to_helio*v;
}

////////////////////////////////////////////////////////////////////////////////
// Move to the given equatorial position
void Navigator::moveTo(const csVector3& _aim, float move_duration, bool _local_pos, int zooming)
{
	zooming_mode = zooming;
	move.aim=_aim;
	move.aim.Normalize();
	move.aim*=2.;
	if (_local_pos)
	{
		move.start=local_vision;
	}
	else
	{
		move.start=equ_vision;
	}
	move.start.Normalize();
	move.speed=1.f/(move_duration*1000);
	move.coef=0.;
	move.local_pos = _local_pos;
	flag_auto_move = true;
}

////////////////////////////////////////////////////////////////////////////////
// Set type of viewing mode (align with horizon or equatorial coordinates)
void Navigator::setViewingMode(VIEWING_MODE_TYPE view_mode)
{
	viewing_mode = view_mode;
}
