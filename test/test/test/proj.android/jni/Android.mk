LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hellocpp_shared

LOCAL_MODULE_FILENAME := libhellocpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/HelloWorldScene.cpp \
                   ../../Classes/VisibleRect.cpp \
                   ../../../../../CSArmature/CSArmature.cpp \
                   ../../../../../CSArmature/CSBone.cpp \
                   ../../../../../CSArmature/animation/CSAnimation.cpp \
                   ../../../../../CSArmature/animation/CSProcessBase.cpp \
                   ../../../../../CSArmature/animation/CSTween.cpp \
                   ../../../../../CSArmature/datas/CSDatas.cpp \
                   ../../../../../CSArmature/display/CSBatchNode.cpp \
                   ../../../../../CSArmature/display/CSDecorativeDisplay.cpp \
                   ../../../../../CSArmature/display/CSDisplayFactory.cpp \
                   ../../../../../CSArmature/display/CSDisplayManager.cpp \
                   ../../../../../CSArmature/display/CSShaderNode.cpp \
                   ../../../../../CSArmature/display/CSSkin.cpp \
                   ../../../../../CSArmature/external_tool/CCTexture2DMutable.cpp \
                   ../../../../../CSArmature/external_tool/GLES-Render.cpp \
                   ../../../../../CSArmature/external_tool/Json/CSContentJsonDictionary.cpp \
                   ../../../../../CSArmature/external_tool/Json/lib_json/json_value.cpp \
                   ../../../../../CSArmature/external_tool/Json/lib_json/json_reader.cpp \
                   ../../../../../CSArmature/external_tool/Json/lib_json/json_writer.cpp \
                   ../../../../../CSArmature/external_tool/tinyxml/tinystr.cpp \
                   ../../../../../CSArmature/external_tool/tinyxml/tinyxml.cpp \
                   ../../../../../CSArmature/external_tool/tinyxml/tinyxmlerror.cpp \
                   ../../../../../CSArmature/external_tool/tinyxml/tinyxmlparser.cpp \
                   ../../../../../CSArmature/physics/CSColliderDetector.cpp \
                   ../../../../../CSArmature/physics/CSPhysicsWorld.cpp \
                   ../../../../../CSArmature/utils/CSArmatureDataManager.cpp \
                   ../../../../../CSArmature/utils/CSDataReaderHelper.cpp \
                   ../../../../../CSArmature/utils/CSSpriteFrameCacheHelper.cpp \
                   ../../../../../CSArmature/utils/CSTransformHelp.cpp \
                   ../../../../../CSArmature/utils/CSTweenFunction.cpp \
                   ../../../../../CSArmature/utils/CSUtilMath.cpp \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
                    $(LOCAL_PATH)/../../../../../CSArmature \
                    $(LOCAL_PATH)/../../../../../CSArmature/animation \
		    $(LOCAL_PATH)/../../../../../CSArmature/datas \
                    $(LOCAL_PATH)/../../../../../CSArmature/display \
                    $(LOCAL_PATH)/../../../../../CSArmature/external_tool \
                    $(LOCAL_PATH)/../../../../../CSArmature/external_tool/Json/json \
                    $(LOCAL_PATH)/../../../../../CSArmature/external_tool/Json \
                    $(LOCAL_PATH)/../../../../../CSArmature/external_tool/Json/lib_json \
                    $(LOCAL_PATH)/../../../../../CSArmature/external_tool/tinyxml \
                    $(LOCAL_PATH)/../../../../../CSArmature/physics \
                    $(LOCAL_PATH)/../../../../../CSArmature/utils 

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static box2d_static

LOCAL_CFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)

$(call import-module, cocos2dx)
$(call import-module, external/Box2D)

