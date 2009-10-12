/*
  Copyright (C) 2009 Development Team of Peragro Tempus

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/**
 * @file eventdata.h
 *
 * @basic Event data container.
 */

#ifndef PT_EVENT_EVENTDATA_H
#define PT_EVENT_EVENTDATA_H

#include <iosfwd>
#include <map>
#include <string>

#include <boost/variant.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include "common/event/eventdata_fwd.h"
#include "common/event/eventid.h"
#include "common/entity/entityid.h"
#include "common/util/exception.h"

#include "common/utility/types.h"
#include "wfmath/point.h"
#include "wfmath/stream.h"
#include "common/util/ptstring.h"
#include "common/utility/colour24.h"

namespace PT
{
  namespace Event
  {
    PT_DEFINE_EXCEPTION_WHAT(AttributeTypeMismatch, "Attribute type mismatch");
    PT_DEFINE_EXCEPTION_WHAT(AttributeNotFound, "Attribute not found");
    PT_DEFINE_ERRORINFO(EventAttrId, EventAttrId);

    /**
     * Event data holder.
     */
    class EventData
    {
    public:
      typedef boost::mpl::vector<int32_t, uint32_t, bool, float,
        std::string, ptString, WFMath::Point<3>, Colour24, EventDataPtr>
        AttributeTypes;

      /// Constructor.
      EventData(const EventId& id);
      /// Constructor.
      EventData(const EventId& id, const Entity::EntityId& entityId);
      /// Copy constructor.
      explicit EventData(const EventData& other);
      /// Assignment operator.
      EventData& operator=(const EventData& event);

      /// Get the event id.
      EventId GetId() const;

      /// Return whether the event is specific to an entity.
      bool IsEntitySpecific() const;
      /// Get the target entity id.
      Entity::EntityId GetEntityId() const;

      /**
       * Get an attribute by name, can throw AttributeNotFound or
       *   AttributeTypeMismatch.
       * @param name Name id.
       * @return Const reference to the attribute.
       */
      template<typename T>
      const T& Get(const EventAttrId& name) const;

    private:
      template<typename T> struct RefAddConst { typedef T Type; };
      template<typename T> struct RefAddConst<T&> { typedef const T& Type; };
    public:
      /**
       * Get an attribute by name. Returns empty if not found or the type is
       *   wrong, using boost::optional.
       * @param name Name id.
       * @return The attribute except if not found or wrong type.
       */
      template<typename T>
      boost::optional<typename RefAddConst<T>::Type> TryGet(
        const EventAttrId& name) const;

      /**
       * Add a named attribute.
       * @param name The name.
       * @param value The attribute value.
       * @return False if the named attribute is already set.
       */
      template<typename T>
      bool Add(const EventAttrId& name, const T& value);

      /**
       * Apply a visitor to each attribute; it must accept all the types in
       *   AttributeTypes as well as EventAttrId. Use WrapRef() or WrapCref()
       *   around the functor if it shouldn't be copied, or it doesn't typedef
       *   result_type.
       * @param functor The visitor functor to apply.
       */
      template<typename F>
      void Apply(F functor) const;

    private:
      typedef boost::make_variant_over<AttributeTypes>::type Attribute;
      typedef std::map<EventAttrId, Attribute, std::less<EventAttrId>,
        boost::fast_pool_allocator<std::pair<const EventAttrId, Attribute> > >
        AttributeMap;
      typedef AttributeMap::const_iterator ConstIterator;

      /// Find attribute, return nullptr if not found.
      const Attribute* Find(const EventAttrId& name) const;

      /// Event identifier.
      EventId id;
      /// Entity id if this event is entity specific, EntityId() otherwise.
      Entity::EntityId entityId;
      /// Map of names to attributes.
      AttributeMap attributes;
    };

    /// Output event data to stream, for testing.
    std::ostream& operator<<(std::ostream& os, const EventData& event);

    /// Combines boost::static_visitor and boost::reference_wrapper.
    template<typename F> struct VisitorWrapper
      : public boost::static_visitor<>, public boost::reference_wrapper<F>
    { VisitorWrapper(F& f) : boost::reference_wrapper<F>(f) {} };

    /// Similar to boost::ref(), allowing functors to be copied shallowly.
    template<typename F> inline VisitorWrapper<F> WrapRef(F& f)
    { return VisitorWrapper<F>(f); }

    /// Similar to boost::cref(), allowing functors to be copied shallowly.
    template<typename F> inline VisitorWrapper<const F> WrapCRef(const F& f)
    { return VisitorWrapper<const F>(f); }

    inline EventId EventData::GetId() const
    { return id; }

    inline bool EventData::IsEntitySpecific() const
    { return entityId.IsValid(); }

    inline Entity::EntityId EventData::GetEntityId() const
    { return entityId; }

    template<typename T>
    const T& EventData::Get(const EventAttrId& name) const
    {
      typedef typename boost::remove_reference<T>::type Type;
      BOOST_MPL_ASSERT((boost::mpl::contains<AttributeTypes, Type>));

      const Attribute* const attr = Find(name);
      if (!attr) throw PT_EX(AttributeNotFound()) << EventAttrIdInfo(name);
      const Type* const result = boost::get<Type>(attr);
      if (!result) throw PT_EX(AttributeTypeMismatch()) << EventAttrIdInfo(name);
      return *result;
    }

    template<typename T>
    boost::optional<typename EventData::RefAddConst<T>::Type> EventData::TryGet(
      const EventAttrId& name) const
    {
      typedef typename boost::remove_reference<T>::type Type;
      BOOST_MPL_ASSERT((boost::mpl::contains<AttributeTypes, Type>));

      const Attribute* const attr = Find(name);
      if (!attr) return boost::none;
      const Type* const result = boost::get<Type>(attr);
      if (!result) return boost::none;
      return *result;
    }

    template<typename T>
    bool EventData::Add(const EventAttrId& name, const T& value)
    {
      BOOST_MPL_ASSERT((boost::mpl::contains<AttributeTypes, T>));

      std::pair<AttributeMap::iterator, bool> result(
        attributes.insert(std::make_pair(name, value)));
      return result.second;
    }

    template<typename F>
    void EventData::Apply(F functor) const
    {
      ConstIterator itr(attributes.begin());
      const ConstIterator end(attributes.end());
      for ( ; itr != end; ++itr)
      {
        functor(itr->first);
        boost::apply_visitor(functor, itr->second);
      }
    }

  } // namespace Event
} // namespace PT

#endif
