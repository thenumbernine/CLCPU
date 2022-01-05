#pragma once

#include <string>
#include <functional>

template<typename IdType>
struct Getter {
	virtual ~Getter() {}
	virtual cl_int getValue(
		// for all getters:
		size_t param_value_size,
		void* param_value,
		size_t* param_value_size_ret,
		// the object we're getting from ... what type ... 
		IdType id
		// any extra params?
		) = 0;
};

template<typename IdType>
struct GetterLambda : public Getter<IdType> {
	std::function<cl_int(size_t, void*, size_t*, IdType)> lambda;
	GetterLambda(decltype(lambda) lambda_) : lambda(lambda_) {}
	virtual cl_int getValue(size_t param_value_size, void* param_value, size_t* param_value_size_ret, IdType id) {
		return lambda(param_value_size, param_value, param_value_size_ret, id);
	}
};

template<typename IdType>
struct GetString : public Getter<IdType> {
	virtual cl_int getValue(size_t param_value_size, void* param_value, size_t* param_value_size_ret, IdType id) {
		std::string result = getString();
		size_t const resultBufSize = result.size() + 1;
		if (param_value_size_ret) {
			param_value_size_ret[0] = resultBufSize;
		}
		if (param_value) {
			if (param_value_size < resultBufSize) {
				return CL_INVALID_VALUE;
			}
			std::copy(result.begin(), result.end(), (char*)param_value);
		}
		return CL_SUCCESS;
	}

	virtual std::string getString() = 0;
};

template<typename IdType>
struct GetStringLiteral : public GetString<IdType> {
	std::string str;
	GetStringLiteral(std::string const str_) : str(str_) {}
	virtual std::string getString() { return str; }
};

template<typename IdType, typename ResultType>
struct GetPrimitive : public Getter<IdType> {
	virtual cl_int getValue(size_t param_value_size, void* param_value, size_t* param_value_size_ret, IdType id) {
		// then handle
		if (param_value_size_ret) {
			param_value_size_ret[0] = sizeof(ResultType);
		}
		if (param_value) {
			if (param_value_size < sizeof(ResultType)) {
				return CL_INVALID_VALUE;
			}
			
			*(ResultType*)param_value = getPrimitive(id);
		}
		return CL_SUCCESS;
	}
	virtual ResultType getPrimitive(IdType id) = 0;
};

template<typename IdType, typename ResultType>
struct GetPrimitiveLiteral : public GetPrimitive<IdType, ResultType> {
	ResultType value;
	GetPrimitiveLiteral(ResultType const value_) : value(value_) {}
	virtual ResultType getPrimitive(IdType id) { return value; }
};

template<typename IdType, typename ResultType>
struct GetPrimitiveFromLambda : public GetPrimitive<IdType, ResultType> {
	std::function<ResultType(IdType)> lambda;
	GetPrimitiveFromLambda(decltype(lambda) lambda_) : lambda(lambda_) {}
	virtual ResultType getPrimitive(IdType id) { return lambda(id); }
};
