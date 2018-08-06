#include <algorithm>

#include "nta/json.h"

namespace nta {
	namespace utils {
		Json::Json(const std::initializer_list<Json>& data) : m_type(NONE) {
			if (data.begin() == data.end()) return;
			bool is_object = std::all_of(data.begin(), data.end(), [](auto& elem) {
				return elem.is_array() && elem.size() == 2 && elem[0].is_string();
			});
			if (is_object) {
				m_type = OBJECT;
				m_obj = new JsonObject;
				for (auto& pair : data) {
					(*m_obj)[pair[0].as_string()] = pair[1];
				}
			} else {
				m_type = ARRAY;
				m_arr = new JsonArray(data.begin(), data.end());
			}
		}
		Json::Json(const Json& other) {
			operator=(other);
		}
		Json::Json(Json&& other) {
			operator=(other);
		}
		Json& Json::operator=(const Json& other) {
			m_type = other.m_type;
			switch(other.m_type) {
			case STRING:
				m_str = strdup(other.m_str);
				m_len = other.m_len;
				break;
			case NUMBER:
				m_num = other.m_num;
				break;
			case OBJECT:
				m_obj = new JsonObject(other.m_obj->begin(), other.m_obj->end());
				break;
			case ARRAY:
				m_arr = new JsonArray(other.m_arr->begin(), other.m_arr->end());
				break;
			case BOOLEAN:
				m_bool = other.m_bool;
				break;
			}
		}
		Json& Json::operator=(Json&& other) {
			m_type = other.m_type;
			switch(other.m_type) {
			case STRING:
				m_str = other.m_str;
				m_len = other.m_len;
				other.m_str = nullptr;
				break;
			case NUMBER:
				m_num = other.m_num;
				break;
			case OBJECT:
				m_obj = other.m_obj;
				other.m_obj = nullptr;
				break;
			case ARRAY:
				m_arr = other.m_arr;
				other.m_arr = nullptr;
				break;
			case BOOLEAN:
				m_bool = other.m_bool;
				break;
			}
			other.m_type = NONE;
		}
		Json::~Json() {
			switch(m_type) {
				case STRING: free(m_str); break;
				case OBJECT: if (m_obj) delete m_obj; break;
				case ARRAY: if (m_arr) delete m_arr; break;
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
		Json& Json::operator[](crstring key) const {
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
		Json& Json::operator[](std::size_t idx) const {
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
		std::string Json::as_string() const {
			switch(m_type) {
				case STRING: return m_str;
				case NUMBER: return m_num.to_string();
				case BOOLEAN: return m_bool ? "true" : "false";
				case NONE: return "null";
			}
			return "";
		}
		JsonNum Json::as_number() const {
			switch(m_type) {
				case NUMBER: return m_num;
				case BOOLEAN: return m_bool;
			}
			return 0;
		}
		bool Json::as_bool() const {
			switch(m_type) {
				case NONE: return false;
				case BOOLEAN: return m_bool;
				default: return true;
			}
		}
		/// \todo Clean up code
		std::string Json::dump(std::size_t indent, std::size_t offset) const {
			switch(m_type) {
				case STRING: return "\"" + as_string() + "\"";
				case NUMBER: return m_num.dump();
				case OBJECT: {
					if (is_empty()) return indent > 0 ? "{\n}" : "{}";
					std::string ret = "{";
					std::string offset_str(offset, ' ');
					std::string indent_str(offset + indent, ' ');

					for (auto it = m_obj->cbegin(); it != m_obj->cend(); ++it) {
						ret += indent == 0 ? "" : "\n" + indent_str;
						ret += "\"" + it->first + "\": " + 
								it->second.dump(indent, offset+indent) + ", ";
					}
					ret.replace(ret.size()-2, 2, indent == 0 ? "}" : 
												 "\n" + offset_str + "}");
					return ret;
				}
				case ARRAY: {
					if (is_empty()) return "[]";
					std::string ret = "[";
					for (auto it = cbegin(); it != cend(); ++it) {
						ret += it->dump() + ", ";
					}
					ret.replace(ret.size()-2, 2, "]");
					return ret;
				}
				case BOOLEAN: return m_bool ? "true" : "false";
				case NONE: return "null";
			}
		}
		Json Json::parse(crstring json) {
			if (json.empty()) return Json::null();
			Json ret;
			return ret;
		}
	}
}