// Unity Native Plugin API copyright © 2015 Unity Technologies ApS
//
// Licensed under the Unity Companion License for Unity - dependent projects--see[Unity Companion License](http://www.unity3d.com/legal/licenses/Unity_Companion_License).
//
// Unless expressly provided otherwise, the Software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.Please review the license for details on these and other terms and conditions.

#pragma once
#include "IUnityInterface.h"
#include "IUnityGraphics.h"

UNITY_DECLARE_INTERFACE(IUnityGraphicsExtensions) {
  virtual ~IUnityGraphicsExtensions() {}

  virtual double GetEstimatedNextCallDuration() = 0;

  virtual void IssuePluginEventAndData(UnityRenderingEventAndData callback, int eventId, void* data) = 0;
};

UNITY_REGISTER_INTERFACE_GUID(0x7CBA0A9CA4DDB545ULL, 0x8C5AD4926EB17B12ULL, IUnityGraphicsExtensions)
