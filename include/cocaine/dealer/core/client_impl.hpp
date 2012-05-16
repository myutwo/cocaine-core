//
// Copyright (C) 2011-2012 Rim Zaidullin <creator@bash.org.ru>
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

#ifndef _COCAINE_DEALER_CLIENT_IMPL_HPP_INCLUDED_
#define _COCAINE_DEALER_CLIENT_IMPL_HPP_INCLUDED_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <zmq.hpp>

#include <boost/function.hpp>
#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "cocaine/dealer/forwards.hpp"
#include "cocaine/dealer/core/context.hpp"
#include "cocaine/dealer/core/service.hpp"
#include "cocaine/dealer/heartbeats/heartbeats_collector.hpp"

namespace cocaine {
namespace dealer {

class client_impl : private boost::noncopyable {
public:
	explicit client_impl(const std::string& config_path);
	virtual ~client_impl();

	std::string send_message(const boost::shared_ptr<message_iface>& msg,
							 const boost::shared_ptr<response>& resp);

	void unset_response_callback(const std::string& message_uuid,
								 const message_path& path);

	boost::shared_ptr<message_iface> create_message(const void* data,
													size_t size,
													const message_path& path,
													const message_policy& policy);

	boost::shared_ptr<base_logger> logger();
	boost::shared_ptr<configuration> config();
	boost::shared_ptr<cocaine::dealer::context> context();

private:
	typedef std::map<std::string, boost::shared_ptr<service_t> > services_map_t;
	typedef std::map<std::string, std::vector<cocaine_endpoint> > handles_endpoints_t;
	
	void connect();
	void disconnect();

	void service_hosts_pinged_callback(const service_info_t& service_info,
									   const handles_endpoints_t& endpoints_for_handles);

	// restoring messages from storage cache
	void load_cached_messages_for_service(boost::shared_ptr<service_t>& service);
	void storage_iteration_callback(void* data, uint64_t size, int column);

private:
	size_t messages_cache_size_;

	// main dealer context
	boost::shared_ptr<cocaine::dealer::context> context_;

	// dealer service name mapped to service
	services_map_t services_;

	// heartsbeat collector
	std::auto_ptr<heartbeats_collector> heartbeats_collector_;

	// temporary service ptr (2 DO: refactor this utter crap)
	boost::shared_ptr<service_t> restored_service_tmp_ptr_;

	// synchronization
	boost::mutex mutex_;

	// alive state
	bool is_dead_;
};

} // namespace dealer
} // namespace cocaine

#endif // _COCAINE_DEALER_CLIENT_IMPL_HPP_INCLUDED_                                                            
