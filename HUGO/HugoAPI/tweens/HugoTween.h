#pragma once

#include <type_traits>

#include "Component.h"
#include "HugoTypes.h"
#include "HugoError.h"
#include "IScene.h"
#include "HugoEasings.h"

namespace HUGO{
    //Base class for abstract pointer in TweenComponent
    class ITween{
        public:
    
        virtual ErrorOrType<bool> tweenParameter(HUGO::IScene& scene, HUGO::Float dt) = 0;
    };
    
    template<typename ClassType, typename EasingType, typename InterpType>
    class Tween: public ITween{
        //The tweenable value ALWAYS is a HUGO::Float
        InterpType ClassType::* m_parameterMemberPt;
        EntityID m_componentEntity;
    
        //Interpolation status
    
        InterpType m_from;
        InterpType m_to;
        HUGO::Float m_totalTime;
        HUGO::Float m_currentTime;

        EasingType m_easingType;
    public:
        inline Tween(ClassType& componentToTween, InterpType ClassType::* parameterMemberPt, InterpType from, InterpType to, HUGO::Float time): 
        m_parameterMemberPt(parameterMemberPt), m_from(from), m_to(to), m_totalTime(time), m_currentTime(0.f), m_easingType(EasingType()){
    
            //Get the entityID associated with componentToTween
            Component& castedComponent = static_cast<Component&>(componentToTween);
            m_componentEntity = castedComponent.entity;
        }
    
        //Apply interpolation value to the referenced HUGO::Float parameter. Returns if Tween has finished or not.
        ErrorOrType<bool> tweenParameter(HUGO::IScene& scene, HUGO::Float dt) override{
            
            m_currentTime += dt;
            if(m_currentTime > m_totalTime) m_currentTime = m_totalTime;
            //->Get interpolation value based on m_currentTime

            //Time value from 0 to 1
            HUGO::Float normalizedTime = m_currentTime / m_totalTime;
            
            //Equivalent from easingFunctions
            HUGO::Float t = m_easingType.easingFunction(normalizedTime);

            //Interpolated value (LERP)
            InterpType value = (m_from * (1.0 - t)) + (m_to * t);
    
            scene.modifyComponent<ClassType>(m_componentEntity,[this, value](ClassType& component){

                component.*m_parameterMemberPt = value;
            });
    
    
            if(m_currentTime >= m_totalTime) return ErrorOrType<bool>(true);
    
            return ErrorOrType<bool>(false);
        }
    };

    //Factory for just using Tween<EasingType>::makeTween(..)
    template <typename EasingType>
    class TweenFactory{
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
        template<typename ClassType, typename InterpType>
        static std::shared_ptr<Tween<ClassType, EasingType, InterpType>> makeTween(ClassType& componentToTween, InterpType ClassType::* parameterMemberPt, InterpType from, InterpType to, HUGO::Float time){
            return std::make_shared<Tween<ClassType, EasingType, InterpType>>(componentToTween, parameterMemberPt, from, to, time);
        }
    };
};