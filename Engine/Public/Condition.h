#pragma once

NS_BEGIN(Engine)

class AnimFSM;

enum class ConditionMode { Equal, Greater, Less, NotEqual };

class ENGINE_DLL Condition 
{
public:
    wstring _paramName;
    ConditionMode _mode;
public:
    virtual bool IsSatisfied(shared_ptr<AnimFSM> fsm) = 0;
};

class ENGINE_DLL FloatCondition : public Condition 
{
public:
    float _value;

    virtual bool IsSatisfied(shared_ptr<AnimFSM> fsm) override;
};

class ENGINE_DLL BoolCondition : public Condition
{
public:
    bool _value;

    virtual bool IsSatisfied(shared_ptr<AnimFSM> fsm) override;
};

NS_END
