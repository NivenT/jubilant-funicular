#include "nta/json.h"

namespace nta {
	namespace utils {
		Json::Json(const Json& other) : m_type(other.m_type) {
			switch(other.m_type) {
			case STRING:
				m_str = strdup(other.m_str);
				m_len = other.m_len;
				break;
			case NUMBER:
				m_num = other.m_num;
				break;
			case OBJECT:
				m_obj = other.m_obj;
				break;
			case ARRAY:
				m_arr = other.m_arr;
				break;
			case BOOLEAN:
				m_bool = other.m_bool;
				break;
			}
		}
		Json& Json::operator[](crstring key) {
			if (m_type == NONE) {
				m_type = OBJECT;
				m_obj = new JsonObject;
			}
			/// \todo Signal error if m_type != OBJECT
			return (*m_obj)[key];
		}
		Json& Json::operator[](std::size_t idx) {
			if (m_type == NONE) {
				m_type = ARRAY;
				m_arr = new JsonArray;
			}
			/// \todo Signal error if m_type != ARRAY
			/// \todo Signal error if idx is out of bounds
			return (*m_arr)[idx];
		}
		std::size_t Json::size() const {
			switch(m_type) {
			case OBJECT:
				return m_obj->size();
			case ARRAY:
				return m_arr->size();
			default:
				return 0;
			}
		}
		bool Json::push_back(const Json& val) {
			if (m_type == NONE) {
				m_type = ARRAY;
				m_arr = new JsonArray;
			}
			if (m_type == ARRAY) m_arr->push_back(val);
			return m_type == ARRAY;
		}
		Json::operator bool() const {
			switch(m_type) {
			case NONE: return false;
			case BOOLEAN: return m_bool;
			default: return true;
			}
		}
	}
}