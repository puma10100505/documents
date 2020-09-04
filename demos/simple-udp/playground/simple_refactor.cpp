#include <cstdio>
#include <iostream>
#include <vector>

enum EPrimitiveType 
{
    EPT_Int,
    EPT_String,
    EPT_Float
};

class MemberVariable
{
public:
    MemberVariable(const char* InName, EPrimitiveType InPrimitiveType, uint32_t InOffset): 
        Name(InName), PrimitiveType(InPrimitiveType), Offset(InOffset) {}

private:
    std::string Name;
    EPrimitiveType PrimitiveType;
    uint32_t Offset;
};

// class DataType
// {
// public:
//     DataType(std::initializer_list<const MemberVariable&> InVars)
//         :MemberVariables(InVars){}

//     const std::vector<MemberVariable>& GetMemberVariables() const {
//         return MemberVariables;
//     }

// private:
//     std::vector<MemberVariable> MemberVariables;
// };

class RuntimeTypeInfo
{
public:
    RuntimeTypeInfo(){
        MemberVariableList.clear();
    }

    void AddMemberInfo(const MemberVariable& NewMember) {
        MemberVariableList.emplace_back(NewMember);
    }

private:
    std::vector<MemberVariable> MemberVariableList;
};


#define PROPERTY(MemberVar) \
{   \
    \
}

class GObject
{
public:
    GObject() {}
    virtual ~GObject() {}

protected:
    RuntimeTypeInfo RTTI;
};

#define OffsetOf(Class, Member) ((size_t) & (static_cast<Class*>(nullptr)->Member))

#define GPROPERTY(DataType, ClassName, PropertyName) \
{ \
  DataType PropertyName; \
  inline void __AddProperty_##PropertyName##(const char* PropertyName, EPrimitiveType Type) { \
    RTTI.AddMemberInfo(MemberVariable(PropertyName, Type, OffsetOf(ClassName, PropertyName)));  \
  }\
}
#define GCLASS(...)

GCLASS()
class GameObject: public GObject
{
public:
    GameObject()
    {
       // InitializeProperties();
    }

    virtual ~GameObject(){}

    // virtual void InitializeProperties() {
    //     //INIT_PROPERTY(Health);
    //     //INIT_PROPERTY(Armor);
    //     RTTI.AddMemberInfo(MemberVariable(Health, EPT_Int, Offset)); 
    // }

private:
    GPROPERTY(int32_t, GameObject, Health)
    //int32_t Health;
    // inline void __AddProperty_Health(const char* PropertyName, EPrimitiveType Type, uint32_t Offset) { 
    //      RTTI.AddMemberInfo(MemberVariable(PropertyName, Type, Offset)); 
    // }

};

int main()
{
    return 0;
}