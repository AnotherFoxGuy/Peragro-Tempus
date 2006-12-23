

#ifndef COORDINATES_H_
#define COORDINATES_H_


void get_sun_helio_coordsv(double jd,double xyz[3]);

void get_mercury_helio_coordsv(double jd,double xyz[3]);
void get_venus_helio_coordsv(double jd,double xyz[3]);
void get_earth_helio_coordsv(double jd,double xyz[3]);
void get_mars_helio_coordsv(double jd,double xyz[3]);
void get_jupiter_helio_coordsv(double jd,double xyz[3]);
void get_saturn_helio_coordsv(double jd,double xyz[3]);
void get_uranus_helio_coordsv(double jd,double xyz[3]);
void get_neptune_helio_coordsv(double jd,double xyz[3]);

void get_mercury_helio_osculating_coords(double jd0,double jd,double xyz[3]);
void get_venus_helio_osculating_coords(double jd0,double jd,double xyz[3]);
void get_earth_helio_osculating_coords(double jd0,double jd,double xyz[3]);
void get_mars_helio_osculating_coords(double jd0,double jd,double xyz[3]);
void get_jupiter_helio_osculating_coords(double jd0,double jd,double xyz[3]);
void get_saturn_helio_osculating_coords(double jd0,double jd,double xyz[3]);
void get_uranus_helio_osculating_coords(double jd0,double jd,double xyz[3]);
void get_neptune_helio_osculating_coords(double jd0,double jd,double xyz[3]);

void get_lunar_parent_coordsv(double jd,double xyz[3]);

void get_phobos_parent_coordsv(double jd,double xyz[3]);
void get_deimos_parent_coordsv(double jd,double xyz[3]);

void get_io_parent_coordsv(double jd,double xyz[3]);
void get_europa_parent_coordsv(double jd,double xyz[3]);
void get_ganymede_parent_coordsv(double jd,double xyz[3]);
void get_callisto_parent_coordsv(double jd,double xyz[3]);

void get_mimas_parent_coordsv(double jd,double xyz[3]);
void get_enceladus_parent_coordsv(double jd,double xyz[3]);
void get_tethys_parent_coordsv(double jd,double xyz[3]);
void get_dione_parent_coordsv(double jd,double xyz[3]);
void get_rhea_parent_coordsv(double jd,double xyz[3]);
void get_titan_parent_coordsv(double jd,double xyz[3]);
void get_hyperion_parent_coordsv(double jd,double xyz[3]);
void get_iapetus_parent_coordsv(double jd,double xyz[3]);

void get_miranda_parent_coordsv(double jd,double xyz[3]);
void get_ariel_parent_coordsv(double jd,double xyz[3]);
void get_umbriel_parent_coordsv(double jd,double xyz[3]);
void get_titania_parent_coordsv(double jd,double xyz[3]);
void get_oberon_parent_coordsv(double jd,double xyz[3]);


#endif /* COORDINATES_H_ */
