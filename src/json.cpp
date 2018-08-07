#include <algorithm>
#include <unordered_set>

#include "nta/json.h"

namespace nta {
	namespace utils {
		Json::Json(const std::initializer_list<Json>& data) : m_type(NONE) {
			if (data.begin() == data.end()) return;
			bool is_object = std::all_of(data.begin(), data.end(), [](const Json& elem) {
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
				case OBJECT: return m_obj->size();
				case ARRAY: return m_arr->size();
				default: return 0;
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
		Json& Json::front() {
			switch(m_type) {
				case OBJECT: return m_obj->begin()->second;
				case ARRAY: return m_arr->front();
				default: return *this;
			}
		}
		Json& Json::back() {
			switch(m_type) {
				case OBJECT: return m_obj->rbegin()->second;
				case ARRAY: return m_arr->back();
				default: return *this;
			}
		}
		Json& Json::merge(const Json& other) {
			if (m_type != other.m_type) return *this;
			switch(m_type) {
				case OBJECT: 
					m_obj->insert(other.m_obj->begin(), other.m_obj->end()); 
					break;
				case ARRAY:
					m_arr->insert(m_arr->end(), other.m_arr->begin(), other.m_arr->end());
					break;
			}
			return *this;
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
						ret += it->dump(indent, offset) + ", ";
					}
					ret.replace(ret.size()-2, 2, "]");
					return ret;
				}
				case BOOLEAN: return m_bool ? "true" : "false";
				case NONE: return "null";
			}
		}
		bool Json::lex_string(std::string& str, JsonToken& ret) {
			if (str[0] != '\"' || str.size() < 2) return false;
			int pos = 0;
			while (pos == 0 || str[pos-1] == '\\') {
				if (pos != 0) str.replace(pos-1, 1, "");
				pos = str.find('\"', pos+1);
				if (pos == std::string::npos) return false;
			}
			ret.type = SYMBOL;
			ret.str = strdup(str.substr(0, pos+1).c_str());
			str = str.substr(pos+1);
			return true;
		}
		bool Json::lex_number(std::string& str, JsonToken& ret) {
			static const std::unordered_set<char> FLOAT_CHARS{
				'.', 'e', 'E'
			};

			char* end;

			long vall = strtol(str.c_str(), &end, 10);
			if (end != str.c_str() && FLOAT_CHARS.find(*end) == FLOAT_CHARS.end()) {
				ret.type = NUMTKN;
				ret.num = vall >= 0 ? JsonNum((uint64_t)vall) : JsonNum((int64_t)vall);
				str = str.substr(end - str.c_str());
				return true;
			}

			double vald = strtod(str.c_str(), &end);
			if (end != str.c_str()) {
				ret.type = NUMTKN;
				ret.num = JsonNum(vald);
				str = str.substr(end - str.c_str());
				return true;
			}

			return false;
		}
		bool Json::lex_bool(std::string& str, JsonToken& ret) {
			if (str.size() >= 4 && str.substr(0, 4) == "true") {
				ret.type = SYMBOL;
				ret.str = strdup("true");
				str = str.substr(4);
				return true;
			} if (str.size() >= 5 && str.substr(0, 5) == "false") {
				ret.type = SYMBOL;
				ret.str = strdup("false");
				str = str.substr(5);
				return true;
			}
			return false;
		}
		bool Json::lex_null(std::string& str, JsonToken& ret) {
			if (str.size() >= 4 && str.substr(0, 4) == "null") {
				ret.type = SYMBOL;
				ret.str = strdup("null");
				str = str.substr(4);
				return true;
			}
			return false;
		}
		std::queue<Json::JsonToken> Json::tokenize(std::string curr) {
			static const std::unordered_set<char> JSON_SYNTAX{
				'{', '}', '[', ']', ':', ','
			};

			std::queue<JsonToken> tokens;
			while (!curr.empty()) {
				JsonToken tkn;
				if (lex_string(curr, tkn)) {
					tokens.push(tkn);
				} else if (lex_number(curr, tkn)) {
					tokens.push(tkn);
				} else if (lex_bool(curr, tkn)) {
					tokens.push(tkn);
				} else if (lex_null(curr, tkn)) {
					tokens.push(tkn);
				} else if (isspace(curr[0])) {
					curr = curr.substr(1);
				} else if (JSON_SYNTAX.find(curr[0]) != JSON_SYNTAX.end()) {
					tokens.emplace(curr[0]);
					//std::string(1, curr[0]));
					curr = curr.substr(1);
				} else {
					/// \todo Error cleanly
					assert(false && "Invalid JSON token");
				}
			}
			return tokens;
		}
		Json Json::parse_tokens(std::queue<JsonToken>& tokens) {
			if (tokens.empty()) return Json::null();
			JsonToken tkn = tokens.front();
			tokens.pop();
			if (tkn.type == NUMTKN) {
				return Json::number(tkn.num);
			} else if (strcmp(tkn.str, "{") == 0) {
				Json obj;
				while (tokens.front() != JsonToken('}')) {
					Json key = parse_tokens(tokens);
					if (!key.is_string()) return Json::null();
					if (tokens.front() != JsonToken(':')) return Json::null();
					tokens.pop();

					obj[key.as_string()] = parse_tokens(tokens);
					if (tokens.front() == JsonToken(',')) tokens.pop();
				}
				tokens.pop();
				return obj;
			} else if (strcmp(tkn.str, "[") == 0) {
				Json arr;
				while (tokens.front() != JsonToken(']')) {
					arr.push_back(parse_tokens(tokens));
					if (tokens.front() == JsonToken(',')) tokens.pop();
				}
				tokens.pop();
				return arr;
			} else if (starts_with(tkn.str, "\"")) {
				int len = strlen(tkn.str);
				tkn.str[len-1] = '\0';
				return Json::string(tkn.str+1);
			} else if (strcmp(tkn.str, "true") == 0) {
				return Json::boolean(true);
			} else if (strcmp(tkn.str, "false") == 0) {
				return Json::boolean(false);
			} else if (strcmp(tkn.str, "null") == 0) {
				return Json::null();
			} else {
				assert(false && "Invalid JSON syntax");
			}
		}
		Json Json::parse(crstring json) {
			if (json.empty()) return Json::null();
			if (isspace(json[0])) return parse(trim(json));

			auto tokens = tokenize(json);
			return parse_tokens(tokens);
		}
		bool Json::operator==(const Json& other) const {
			if (m_type != other.m_type) return false;
			switch(m_type) {
				case STRING: return strcmp(m_str, other.m_str) == 0;
				case NUMBER: return m_num == other.m_num;
				case OBJECT: return m_obj ? (other.m_obj && *m_obj == *other.m_obj) : !other.m_obj;
				case ARRAY: return m_arr ? (other.m_arr && *m_arr == *other.m_arr) : !other.m_arr;
				case BOOLEAN: return m_bool == other.m_bool;
				case NONE: return true;
			}
		}
	}
}