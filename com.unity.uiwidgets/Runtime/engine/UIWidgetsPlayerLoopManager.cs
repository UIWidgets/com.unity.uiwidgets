using System.Collections.Generic;
using System.Text;
using UnityEngine;
using UnityEngine.LowLevel;
using UnityEngine.PlayerLoop;
using static UnityEngine.PlayerLoop.PostLateUpdate;
using static UnityEngine.PlayerLoop.Update;
using UnityEditor;

public class UIWidgetUpdate { }
public class UIWidgetVSync { }
public class UIWidgetWait { }

namespace Unity.UIWidgets.engine
{
    [InitializeOnLoad]
    public class UIWidgetsPlayerLoopManager
    {

        private static PlayerLoopSystem uiwidgetUpdate = CreateCustomUpdateSystem(typeof(UIWidgetUpdate), UIWidgetsPanelWrapper.UIWidgetsSystem_SendUpdateEvent);
        private static PlayerLoopSystem uiwidgetVSync = CreateCustomUpdateSystem(typeof(UIWidgetVSync), UIWidgetsPanelWrapper.UIWidgetsSystem_SendVSyncEvent);
        private static PlayerLoopSystem uiwidgetWait = CreateCustomUpdateSystem(typeof(UIWidgetWait), UIWidgetsPanelWrapper.UIWidgetsSystem_SendWaitEvent);
        static UIWidgetsPlayerLoopManager()
        {
            StartApp();
        }

        public static void StartApp()
        {
            // Retrieve the default Player loop system. Get the current loop instead if the default was already modified previously.
            var defaultLoop = PlayerLoop.GetDefaultPlayerLoop();

            var loopWithCustomUpdate = UpdateWithUIWidgetsPlayerLoop(defaultLoop);
            PlayerLoop.SetPlayerLoop(loopWithCustomUpdate);

            // Print the current Player loop to verify the custom update was added
            StringBuilder sb = new();
            RecursivePlayerLoopPrint(PlayerLoop.GetCurrentPlayerLoop(), sb, 0);
            Debug.Log(sb.ToString());
        }

        private static PlayerLoopSystem CreateCustomUpdateSystem(System.Type type, PlayerLoopSystem.UpdateFunction updateDelegate)
        {
            return new PlayerLoopSystem
            {
                subSystemList = null,
                updateDelegate = updateDelegate,
                type = type
            };
        }

        private static PlayerLoopSystem InsertSystemAfter<T>(in PlayerLoopSystem loopSystem, PlayerLoopSystem newSystem) where T : struct
        {
            // Create a new root PlayerLoopSystem
            PlayerLoopSystem newPlayerLoop = new()
            {
                loopConditionFunction = loopSystem.loopConditionFunction,
                type = loopSystem.type,
                updateDelegate = loopSystem.updateDelegate,
                updateFunction = loopSystem.updateFunction
            };
            // Create a new list to populate with subsystems, including the custom system
            List<PlayerLoopSystem> newSubSystemList = new();

            //Iterate through the subsystems in the existing loop we passed in and add them to the new list
            if (loopSystem.subSystemList != null)
            {
                for (var i = 0; i < loopSystem.subSystemList.Length; i++)
                {
                    newSubSystemList.Add(loopSystem.subSystemList[i]);
                    // If the previously added subsystem is of the type to add after, add the custom system
                    if (loopSystem.subSystemList[i].type == typeof(T))
                    {
                        newSubSystemList.Add(newSystem);
                    }
                }
            }

            newPlayerLoop.subSystemList = newSubSystemList.ToArray();
            return newPlayerLoop;
        }

        private static PlayerLoopSystem UpdateWithUIWidgetsPlayerLoop(in PlayerLoopSystem loopSystem)
        {
            // Create a new root PlayerLoopSystem
            PlayerLoopSystem newPlayerLoop = new()
            {
                loopConditionFunction = loopSystem.loopConditionFunction,
                type = loopSystem.type,
                updateDelegate = loopSystem.updateDelegate,
                updateFunction = loopSystem.updateFunction
            };
            // Create a new list to populate with subsystems, including the custom system
            List<PlayerLoopSystem> newSubSystemList = new();

            //Iterate through the subsystems in the existing loop we passed in and add them to the new list
            if (loopSystem.subSystemList != null)
            {
                for (var i = 0; i < loopSystem.subSystemList.Length; i++)
                {
                    if (loopSystem.subSystemList[i].type == typeof(Update))
                    {
                        var loopWithCustomUpdate = InsertSystemAfter<ScriptRunDelayedTasks>(in loopSystem.subSystemList[i], uiwidgetUpdate);
                        newSubSystemList.Add(loopWithCustomUpdate);
                    }
                    else if (loopSystem.subSystemList[i].type == typeof(PostLateUpdate))
                    {
                        var loopWithCustomUpdate = InsertSystemAfter<PhysicsSkinnedClothFinishUpdate>(in loopSystem.subSystemList[i], uiwidgetVSync);
                        var loopWithCustomUpdate2 = InsertSystemAfter<GraphicsWarmupPreloadedShaders>(in loopWithCustomUpdate, uiwidgetWait);
                        newSubSystemList.Add(loopWithCustomUpdate2);
                    }
                    else
                    {
                        newSubSystemList.Add(loopSystem.subSystemList[i]);
                    }
                }
            }

            newPlayerLoop.subSystemList = newSubSystemList.ToArray();
            return newPlayerLoop;
        }

        private static void RecursivePlayerLoopPrint(PlayerLoopSystem playerLoopSystem, StringBuilder sb, int depth)
        {
            if (depth == 0)
            {
                sb.AppendLine("ROOT NODE");
            }
            else if (playerLoopSystem.type != null)
            {
                for (int i = 0; i < depth; i++)
                {
                    sb.Append("\t");
                }
                sb.AppendLine(playerLoopSystem.type.Name);
            }
            if (playerLoopSystem.subSystemList != null)
            {
                depth++;
                foreach (var s in playerLoopSystem.subSystemList)
                {
                    RecursivePlayerLoopPrint(s, sb, depth);
                }
                depth--;
            }
        }
    }
}