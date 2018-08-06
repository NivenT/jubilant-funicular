#include "nta/json.h"

namespace nta {
	namespace utils {
		Json::Json(const Json& other) : m_type(other.m_type) {
			switch(other.m_type) {
			case STRING:
				m_str = other.m_str;
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
		Json& Json::operator=(const Json& other) {
			// This feels wrong
			*this = Json(other);
		}
		Json& Json::operator[](crstring key) {
			if (m_type == NONE) {
				m_type = OBJECT;
				m_obj.clear();
			}
			/// \todo Signal error if m_type != OBJECT
			return m_obj[key];
		}
		Json& Json::operator[](std::size_t idx) {
			if (m_type == NONE) {
				m_type = ARRAY;
				m_arr.clear();
			}
			/// \todo Signal error if m_type != ARRAY
			return m_arr[idx];
		}
	}
}