//
// Copyright (C) 2011-2012 Andrey Sibiryov <me@kobology.ru>
//
// Licensed under the BSD 2-Clause License (the "License");
// you may not use this file except in compliance with the License.
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef COCAINE_REPOSITORY_HPP
#define COCAINE_REPOSITORY_HPP

#include <typeinfo>
#include <boost/type_traits/is_base_of.hpp>

#include <ltdl.h>

#include "cocaine/common.hpp"

namespace cocaine {

struct factory_concept_t {
    virtual const std::type_info& category() const = 0;
};

template<class Category>
struct category_traits;

// Custom factories should inherit from this interface.
template<class Category>
struct factory:
    public factory_concept_t
{
    typedef typename category_traits<Category>::ptr_type ptr_type;
    typedef typename category_traits<Category>::args_type args_type;

    virtual const std::type_info& category() const {
        return typeid(Category);
    }

    virtual ptr_type get(context_t& context,
                         const args_type& args) = 0;
};

// Specialize this in your plugin to use
// a custom factory for object instatiations.
template<class T>
struct plugin_traits {
    typedef typename category_traits<typename T::category_type>::template default_factory<T> factory_type;
};

// Component repository
// --------------------

class repository_t:
    public boost::noncopyable
{
    public:
        repository_t(context_t& context);
        ~repository_t();

        template<class Category>
        typename category_traits<Category>::ptr_type
        get(const std::string& type,
            const typename category_traits<Category>::args_type& args)
        {
            factory_map_t::iterator it(m_factories.find(type));

            if(it == m_factories.end()) {
                throw registry_error_t("plugin '" + type + "' is not available");
            }

            if(it->second->category() != typeid(Category)) {
                throw registry_error_t("plugin '" + type + "' has an incompatible type");
            }

            return typename category_traits<Category>::ptr_type(
                dynamic_cast< factory<Category>* >(
                    it->second
                )->get(m_context, args)
            );
        }

        template<class T>
        typename boost::enable_if<
            boost::is_base_of<typename T::category_type, T>
        >::type 
        insert(const std::string& type) {
            if(m_factories.find(type) != m_factories.end()) {
                throw registry_error_t("duplicate plugin '" + type + "' has been detected");
            }

            m_factories.insert(
                type,
                new typename plugin_traits<T>::factory_type()
            );
        }

    private:
        context_t& m_context;
        boost::shared_ptr<logging::logger_t> m_log;

        // Used to unload all the plugins on shutdown.
        std::vector<lt_dlhandle> m_plugins;
    
#if BOOST_VERSION >= 104000
        typedef boost::ptr_unordered_map<
#else
        typedef boost::ptr_map<
#endif
            const std::string, 
            factory_concept_t
        > factory_map_t;
        
        factory_map_t m_factories;
};

}

#endif
