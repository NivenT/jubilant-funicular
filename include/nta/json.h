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
		enum JsonValueType {
			STRING,
			NUMBER,
			OBJECT,
			ARRAY,
			BOOLEAN,
			NONE // Should be called null
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
			JsonNumType get_type() { return m_type; }
			/// \todo Signal ErrorManager if wrong type
			uint64_t as_uint() { return m_pos; }
			int64_t as_int() { return m_neg; }
			double as_float() { return m_flt; }
			double as_double() { return m_flt; }
		};
		class Json {
		private:
			JsonValueType m_type;
			union {
				std::string m_str;
				JsonNum m_num;
				JsonObject m_obj;
				JsonArray m_arr;
				bool m_bool;
			};
		public:
			Json() : m_type(NONE) {}
			Json(crstring str) : m_type(STRING), m_str(str) {}
			Json(JsonNum num) : m_type(NUMBER), m_num(num) {}
			Json(JsonObject obj) : m_type(OBJECT), m_obj(obj) {}
			Json(JsonArray arr) : m_type(ARRAY), m_arr(arr) {}
			Json(bool b) : m_type(BOOLEAN), m_bool(b) {}
			Json(const Json& other);
			~Json() {}
			static Json null() { return {NONE}; }
			JsonValueType get_type() { return m_type; }
			std::string as_string() { return m_str; }
			JsonNum as_number() { return m_num; }
			JsonObject as_object() { return m_obj; }
			JsonArray as_array() { return m_arr; }
			bool as_bool() { return m_bool; }

			Json& operator=(const Json& other);
			Json& operator[](crstring key);
			Json& operator[](std::size_t idx);
		};
	}
}

#endif // NTA_JSON_H_INCLUDED