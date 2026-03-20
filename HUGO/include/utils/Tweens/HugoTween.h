#pragma once
#include "Component.h"
#include "HugoMath.h"
#include "HugoError.h"
#include "IScene.h"
#include <type_traits>
#include <HugoEasings.h>

namespace HUGO{
    //Base class for abstract pointer in TweenComponent
    class ITween{
        public:
    
        virtual ErrorOrType<bool> tweenParameter(IScene& scene, HUGO::Float dt) = 0;
    };
    
    template<typename ClassType, typename EasingType>
    class Tween: public ITween{
        //The tweenable value ALWAYS is a HUGO::Float
        HUGO::Float ClassType::* m_parameterMemberPt;
        EntityID m_componentEntity;
    
        //Interpolation status
    
        HUGO::Float m_from;
        HUGO::Float m_to;
        HUGO::Float m_totalTime;
        HUGO::Float m_currentTime;

        EasingType m_easingType;
    public:
        /*
            Create a Tween. It must be appended to the vector of the TweenComponent.
            @typedef EasingType The easing that will be performed. See them at HugoEasings.h
            @param componentToTween A reference of the component which will be Tweened
            @param parameterMemberPt The FLOAT parameter of the component that will be Tweened.  
            @param from Starting value of the Tween
            @param to Ending value of the Tween
            @param time Duration of the Tween
            @param easing Easing function that will be used
        */
        inline Tween(ClassType& componentToTween, HUGO::Float ClassType::* parameterMemberPt, HUGO::Float from, HUGO::Float to, HUGO::Float time): 
        m_parameterMemberPt(parameterMemberPt), m_from(from), m_to(to), m_totalTime(time), m_currentTime(0.f), m_easingType(EasingType()){
    
            //Get the entityID associated with componentToTween
            Component& castedComponent = static_cast<Component&>(componentToTween);
            m_componentEntity = castedComponent.entity;
        }
    
        //Apply interpolation value to the referenced HUGO::Float parameter. Returns if Tween has finished or not.
        ErrorOrType<bool> tweenParameter(IScene& scene, HUGO::Float dt) override{
            
            m_currentTime += dt;
            if(m_currentTime > m_totalTime) m_currentTime = m_totalTime;
            //->Get interpolation value based on m_currentTime

            //Time value from 0 to 1
            HUGO::Float normalizedTime = m_currentTime / m_totalTime;
            
            HUGO::Float value = m_easingType.easingFunction(normalizedTime);
    
            ClassType& component = scene.getComponent<ClassType>(m_componentEntity);
            component.*m_parameterMemberPt = value;
    
    
            if(m_currentTime >= m_totalTime) return ErrorOrType<bool>(true);
    
            return ErrorOrType<bool>(false);
        }
    };

    //Factory for just using makeTween<EasingType>(..)
    template <typename EasingType>
    class Tween{
    public:
        template<typename ClassType>
        static Tween<ClassType, EasingType> makeTween(ClassType& componentToTween, HUGO::Float ClassType::* parameterMemberPt, HUGO::Float from, HUGO::Float to, HUGO::Float time){
            return Tween<ClassType, EasingType>(componentToTween, parameterMemberPt, from, to, time);
        }
    };
};