#pragma once

namespace zonetool::s2
{
	struct dlog_schema_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(DLogSchema, name));
			w.entries(offsetof(DLogSchema, events), sizeof(DLogEvent), w.template at<int>(offsetof(DLogSchema, eventCount)), [](const auto& event, std::size_t)
			{
				event.string(offsetof(DLogEvent, name));
				event.entries(offsetof(DLogEvent, fields), sizeof(DLogField), event.template at<int>(offsetof(DLogEvent, fieldCount)), [](const auto& field, std::size_t)
				{
					field.string(offsetof(DLogField, name));
					field.string(offsetof(DLogField, typeName));
					field.entries(offsetof(DLogField, values), sizeof(DLogEnumValue),
						field.template at<int>(offsetof(DLogField, valueCount)), [](const auto& value, std::size_t)
					{
						value.string(offsetof(DLogEnumValue, name));
					});
					dlog_schema_layout::visit_hash_table(field.sub(offsetof(DLogField, hashTable)));
				});
				dlog_schema_layout::visit_hash_table(event.sub(offsetof(DLogEvent, hashTable)));
			});
			visit_hash_table(w.sub(offsetof(DLogSchema, hashTable)));
		}

	private:
		template <typename V>
		static void visit_hash_table(const V& table)
		{
			table.bytes(offsetof(DLogHashTable, list), 3, 8 * table.template at<int>(offsetof(DLogHashTable, count)));
		}
	};

	class dlog_schema : public field_layout_asset<DLogSchema, ASSET_TYPE_DLOG_SCHEMA, dlog_schema_layout>
	{
	};

	struct dlog_routes_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(DLogRoutes, name));
			w.entries(offsetof(DLogRoutes, destinations), sizeof(DLogDestination),
				w.template at<int>(offsetof(DLogRoutes, destinationCount)), [](const auto& destination, std::size_t)
			{
				dlog_routes_layout::visit_destination(destination);
			});
			w.entries(offsetof(DLogRoutes, routes), sizeof(DLogRoute), w.template at<int>(offsetof(DLogRoutes, routeCount)), [](const auto& route, std::size_t)
			{
				route.string(offsetof(DLogRoute, name));
				route.entries(offsetof(DLogRoute, entries), sizeof(DLogRouteEntry),
					route.template at<int>(offsetof(DLogRoute, entryCount)), [](const auto& entry, std::size_t)
				{
					entry.entries(offsetof(DLogRouteEntry, destination), sizeof(DLogDestination), 1, [](const auto& destination, std::size_t)
					{
						dlog_routes_layout::visit_destination(destination);
					});
					entry.string(offsetof(DLogRouteEntry, name));
				});
			});
		}

	private:
		template <typename V>
		static void visit_destination(const V& destination)
		{
			destination.strings({offsetof(DLogDestination, name), offsetof(DLogDestination, type), offsetof(DLogDestination, address)});
		}
	};

	class dlog_routes : public field_layout_asset<DLogRoutes, ASSET_TYPE_DLOG_ROUTES, dlog_routes_layout>
	{
	};
}
