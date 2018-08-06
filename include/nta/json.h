#ifndef NTA_JSON_H_INCLUDED
#define NTA_JSON_H_INCLUDED

#include "nta/MyEngine.h"

#include <map>
#include <vector>

namespace nta {
	namespace utils {
		class Json;
		typedef std::map<std::string, Json> JsonObject;
		typedef std::vector<Json> JsonArray;

		enum JsonNumType {
			POS_INT,
			NEG_INT,
			FLOAT
		};
		class JsonNum {
		private:
			JsonNumType m_type;
			union {
				uint64_t m_pos;
				int64_t m_neg;
				double m_flt;
			};
		public:
			JsonNum() : m_type(POS_INT), m_pos(0) {}
			JsonNum(uint64_t n) : m_type(POS_INT), m_pos(n) {}
			JsonNum(int64_t n) : m_type(NEG_INT), m_neg(n) {}
			JsonNum(double n) : m_type(FLOAT), m_flt(n) {}
			JsonNum(int n) : m_type(n >= 0 ? POS_INT : NEG_INT) {
				if (m_type == POS_INT) {
					m_pos = n;
				} else {
					m_neg = n;
				}
			}
			JsonNumType get_type() { return m_type; }
			/// \todo Signal ErrorManager if wrong type
			uint64_t as_uint() { return m_pos; }
			int64_t as_int() { return m_neg; }
			double as_float() { return m_flt; }
			double as_double() { return m_flt; }
		};

		enum JsonValueType {
			STRING,
			NUMBER,
			OBJECT,
			ARRAY,
			BOOLEAN,
			NONE // Should be called null
		};
		class Json {
		private:
			Json(JsonValueType type) : m_type(type) {}

			JsonValueType m_type;
			union {
				struct {
					char* m_str;
					std::size_t m_len;
				};
				JsonNum m_num;
				JsonObject* m_obj;
				JsonArray* m_arr;
				bool m_bool;
			};
		public:
			Json() : m_type(NONE) {}
			Json(crstring str) : m_type(STRING), m_str(strdup(str.c_str())), m_len(str.size()) {}
			Json(const char* str) : m_type(STRING), m_str(strdup(str)), m_len(strlen(str)) {}
			Json(JsonNum num) : m_type(NUMBER), m_num(num) {}
			Json(int num) : m_type(NUMBER), m_num(num) {}
			Json(JsonObject obj) : m_type(OBJECT), m_obj(new JsonObject(obj)) {}
			Json(JsonArray arr) : m_type(ARRAY), m_arr(new JsonArray(arr)) {}
			Json(bool b) : m_type(BOOLEAN), m_bool(b) {}
			Json(const Json& other);
			~Json() {}

			static Json string(crstring str) { 
				Json s(STRING); 
				s.m_str = strdup(str.c_str());
				s.m_len = str.size(); 
				return s; 
			}
			static Json string(const char* str) { 
				Json s(STRING); 
				s.m_str = strdup(str);
				s.m_len = strlen(str);
				return s; 
			}
			static Json number(JsonNum num) { 
				Json n(NUMBER); 
				n.m_num = num; 
				return n; 
			}
			static Json object(JsonObject obj) { 
				Json o(OBJECT); 
				o.m_obj = new JsonObject(obj); 
				return o; 
			}
			static Json array(JsonArray arr) { 
				Json a(ARRAY); 
				a.m_arr = new JsonArray(arr); 
				return a; 
			}
			static Json boolean(bool boolean) { 
				Json b(BOOLEAN); 
				b.m_bool = boolean; 
				return b; 
			}
			static Json null() { return Json(NONE); }

			JsonValueType get_type() const { return m_type; }
			bool is_string() const { return m_type == STRING; }
			bool is_number() const { return m_type == NUMBER; }
			bool is_object() const { return m_type == OBJECT; }
			bool is_array() const { return m_type == ARRAY; }
			bool is_bool() const { return m_type == BOOLEAN; }
			bool is_null() const { return m_type == NONE; }

			std::string as_string() const { return m_str; }
			JsonNum as_number() const { return m_num; }
			uint64_t as_uint() { return m_num.as_uint(); }
			int64_t as_int() { return m_num.as_int(); }
			double as_float() { return m_num.as_float(); }
			double as_double() { return m_num.as_double(); }
			JsonObject as_object() const { return *m_obj; }
			JsonArray as_array() const { return *m_arr; }
			bool as_bool() const { return m_bool; }

			std::size_t size() const;

			/// Returns false is m_type != ARRAY (or NONE)
			bool push_back(const Json& val);

			operator bool() const;
			Json& operator[](crstring key);
			Json& operator[](const char* key) { return operator[](std::string(key)); }
			Json& operator[](std::size_t idx);
			Json& operator[](int idx) { return operator[]((std::size_t)idx); }
		};
	}
}

#endif // NTA_JSON_H_INCLUDED