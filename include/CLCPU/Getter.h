#pragma once

#include "Common/Meta.h"
#include <string>
#include <functional>
#include <type_traits>
#include <memory>

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

//down this child tree, IdType and id are never used ...
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
struct GetStringLiteralClass : public GetString<IdType> {
	std::string str;
	GetStringLiteralClass(std::string const str_) : str(str_) {}
	virtual std::string getString() { return str; }
};

template<typename IdType>
auto GetStringLiteral(std::string str) {
	return std::make_shared<GetStringLiteralClass<IdType>>(str);
}

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
struct GetPrimitiveLiteralClass : public GetPrimitive<IdType, ResultType> {
	ResultType value;
	GetPrimitiveLiteralClass(ResultType const value_) : value(value_) {}
	virtual ResultType getPrimitive(IdType id) { return value; }
};

//is IdType really needed here?  only to cast the id object in the parent ... which is passed to the child ... and never used here ...
template<typename IdType, typename ResultType>
auto GetPrimitiveLiteral(ResultType value) {
	return std::make_shared<GetPrimitiveLiteralClass<IdType, ResultType>>(value);
}

template<typename IdType, typename ResultType>
struct GetPrimitiveFromLambda : public GetPrimitive<IdType, ResultType> {
	std::function<ResultType(IdType)> lambda;
	GetPrimitiveFromLambda(decltype(lambda) lambda_) : lambda(lambda_) {}
	virtual ResultType getPrimitive(IdType id) { return lambda(id); }
};

//assumes IdType is a pointer of a struct that holds the field
template<typename IdType, typename ResultType>
struct GetFieldClass : public GetPrimitive<
	IdType,
	ResultType
> {
	using IdBaseType = std::remove_pointer_t<IdType>;
	ResultType IdBaseType::*field = {};
	GetFieldClass(
		ResultType IdBaseType::* field_
	) : field(field_) {}
	virtual ResultType getPrimitive(IdType id) {
		return id->*field;
	}
};

template<typename MemberPtrType>
auto GetField(MemberPtrType t) {
	using IdBaseType = typename Common::MemberPointer<MemberPtrType>::ClassType;
	using IdType = IdBaseType*;
	using ResultType = typename Common::MemberPointer<MemberPtrType>::FieldType;
	using GetFieldType = GetFieldClass<IdType, ResultType>;
	return std::make_shared<GetFieldType>(t);
}
