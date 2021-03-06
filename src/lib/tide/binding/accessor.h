/**
 * Copyright (c) 2012 - 2014 TideSDK contributors 
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifndef _ACCESSOR_H_
#define _ACCESSOR_H_

namespace tide
{
    typedef std::map<std::string, TiMethodRef> AccessorMap;

    class TIDE_API Accessor
    {
    protected:
        Accessor() {}

        inline void RecordAccessor(const std::string& name, ValueRef value)
        {
            if (name.find("set") == 0)
                DoMap(name.substr(3), value, setterMap);

            else if (name.find("get") == 0)
                DoMap(name.substr(3), value, getterMap);

            else if (name.find("is") == 0)
                DoMap(name.substr(2), value, getterMap);
        }

        bool HasGetterFor(std::string name)
        {
            return !FindAccessor(name, getterMap).isNull();
        }

        ValueRef UseGetter(std::string name, ValueRef existingValue)
        {
            if (!existingValue->IsUndefined())
                return existingValue;

            TiMethodRef getter = FindAccessor(name, getterMap);
            if (getter.isNull())
                return existingValue;

            return getter->Call();
        }

        bool UseSetter(std::string name, ValueRef newValue, ValueRef existingValue)
        {
            RecordAccessor(name, newValue);

            // If a property already exists on this object with the given
            // name, just set the property and don't call the setter.
            if (!existingValue->IsUndefined())
                return false;

            TiMethodRef setter = FindAccessor(name, setterMap);
            if (setter.isNull())
                return false;

            setter->Call(newValue);
            return true;
        }

    private:
        inline void DoMap(std::string name, ValueRef accessor, AccessorMap& map)
        {
            // Lower-case the name so that all comparisons are case-insensitive.
            std::transform(name.begin(), name.end(), name.begin(), tolower);

            // Null old mapping if it exists. This is so that if an accessor
            // is replaced with a non-accessor, we don't keep a copy of it around.
            if (map.find(name) != map.end())
                map[name] = 0;

            if (!accessor->IsMethod())
                return;

            map[name] = accessor->ToMethod();
        }

        inline TiMethodRef FindAccessor(std::string& name, AccessorMap& map)
        {
            // Lower-case the name so that all comparisons are case-insensitive.
            std::transform(name.begin(), name.end(), name.begin(), tolower);

            if (map.find(name) == map.end())
                return 0;

            return map[name];
        }

        DISALLOW_EVIL_CONSTRUCTORS(Accessor);
        AccessorMap getterMap;
        AccessorMap setterMap;
    };
}

#endif
