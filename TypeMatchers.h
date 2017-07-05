#include <clang/AST/Decl.h>

template <typename T>
bool isConstCharPtrType(T* expr)
{
    auto qualType = expr->getType();
    auto bareType = qualType.split().Ty;

    bool isConst = false;
    bool isVolatile = false;
    bool isPointer = bareType->isPointerType();

    bool isPointeeChar = false;
    if (isPointer)
    {
        isPointeeChar = bareType->getPointeeType()->isCharType();
        isConst = bareType->getPointeeType().isConstQualified();
        isVolatile = bareType->getPointeeType().isVolatileQualified();
    }

    return isConst && !isVolatile && isPointer && isPointeeChar;
}

template <typename T>
bool isQObjectPtrType(T *expr)
{
    auto qualType = expr->getType();
    auto bareType = qualType.split().Ty;

    bool isConst = qualType.isConstQualified();
    bool isVolatile = qualType.isVolatileQualified();
    bool isPointer = bareType->isPointerType();

    bool isPointeeQObject = false;
    if (isPointer)
    {
        auto identifier = bareType->getPointeeType().getBaseTypeIdentifier();
        if (identifier)
            isPointeeQObject = identifier->getName() == "QObject";
    }

    return !isConst && !isVolatile && isPointer && isPointeeQObject;
}
