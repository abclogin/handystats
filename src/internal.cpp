#include <handystats/internal_metrics_impl.hpp>
#include "handystats/internal_impl.hpp"

namespace handystats { namespace internal {

std::unordered_map<std::string, internal_metric> internal_metrics;


void process_event_message(events::event_message* message, internal_metric& metric) {
	switch (metric.which()) {
		case internal_metric_index::INTERNAL_COUNTER:
			boost::get<internal_counter*>(metric)->process_event_message(message);
			break;
		case internal_metric_index::INTERNAL_GAUGE:
			boost::get<internal_gauge*>(metric)->process_event_message(message);
			break;
		case internal_metric_index::INTERNAL_TIMER:
			boost::get<internal_timer*>(metric)->process_event_message(message);
			break;
		default:
			return;
	}
}

void process_event_message(events::event_message* message) {
	if (!message) {
		return;
	}

	if (internal_metrics.find(message->destination_name) == internal_metrics.end()) {
		switch (message->destination_type) {
			case events::event_destination_type::COUNTER:
				internal_metrics[message->destination_name] = new internal_counter();
				break;
			case events::event_destination_type::GAUGE:
				internal_metrics[message->destination_name] = new internal_gauge();
				break;
			case events::event_destination_type::TIMER:
				internal_metrics[message->destination_name] = new internal_timer();
				break;
			default:
				return;
		}
	}
	auto& metric = internal_metrics[message->destination_name];
	process_event_message(message, metric);
}


void initialize() {
}

void finalize() {
	for (auto metric_entry : internal_metrics) {
		switch (metric_entry.second.which()) {
			case internal_metric_index::INTERNAL_COUNTER:
				delete boost::get<internal_counter*>(metric_entry.second);
				break;
			case internal_metric_index::INTERNAL_GAUGE:
				delete boost::get<internal_gauge*>(metric_entry.second);
				break;
			case internal_metric_index::INTERNAL_TIMER:
				delete boost::get<internal_timer*>(metric_entry.second);
				break;
			default:
				return;
		}
	}

	internal_metrics.clear();
}

}} // namespace handystats::internal
