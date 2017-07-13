#include "TypeMatchers.h"
#include <ContextMatchers.h>
#include <clang/AST/DeclCXX.h>

bool isConstCharPtrType(const clang::QualType& type)
{
    if (type.getTypePtr()->isPointerType())
    {
        auto pointee = type.getTypePtr()->getPointeeType();
        auto quals = pointee.getQualifiers();

        bool isPointeeChar = pointee.getTypePtr()->isCharType();
        return quals.hasConst() && !quals.hasVolatile() && isPointeeChar;
    }
    return false;
}

bool isQObjectPtrType(const clang::QualType& type)
{
    if (type.getTypePtr()->isPointerType())
    {
        auto pointee = type.getTypePtr()->getPointeeType();
        auto identifier = pointee.getBaseTypeIdentifier();
        auto quals = pointee.getQualifiers();

        bool isPointeeQObject = false;
        if (identifier)
            isPointeeQObject = identifier->getName() == "QObject";
        return quals.hasConst() && !quals.hasVolatile() && isPointeeQObject;
    }
    return false;
}

bool isQtConnectionTypeEnum(const clang::QualType& type)
{
    bool isEnum = type.getTypePtr()->isEnumeralType();
    auto identifier = type.getBaseTypeIdentifier();
    if (identifier)
    {
        std::string name = identifier->getName();
        bool isNameSpaceCorrect = isEnumInQtNamespace(type);
        return isEnum && name == "ConnectionType" && isNameSpaceCorrect;
    }
    return false;
}

