#include "TypeMatchers.h"

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
