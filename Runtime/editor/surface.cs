using System;
using Unity.UIWidgets.foundation;
using Unity.UIWidgets.ui;
using UnityEngine;
using Canvas = Unity.UIWidgets.ui.Canvas;
using Object = UnityEngine.Object;
using Rect = UnityEngine.Rect;

namespace Unity.UIWidgets.editor {
    public delegate bool SubmitCallback(SurfaceFrame surfaceFrame, Canvas canvas);

    public class SurfaceFrame : IDisposable {
        bool _submitted;

        readonly GrSurface _surface;

        readonly SubmitCallback _submitCallback;

        public SurfaceFrame(GrSurface surface, SubmitCallback submitCallback) {
            this._surface = surface;
            this._submitCallback = submitCallback;
        }

        public void Dispose() {
            if (this._submitCallback != null && !this._submitted) {
                this._submitCallback(this, null);
            }
        }

        public Canvas getCanvas() {
            return this._surface != null ? this._surface.getCanvas() : null;
        }

        public bool submit() {
            if (this._submitted) {
                return false;
            }

            this._submitted = this._performSubmit();

            return this._submitted;
        }

        bool _performSubmit() {
            if (this._submitCallback == null) {
                return false;
            }

            if (this._submitCallback(this, this.getCanvas())) {
                return true;
            }

            return false;
        }
    }

    public interface Surface : IDisposable {
        SurfaceFrame acquireFrame(Size size, double devicePixelRatio);
    }

    public class EditorWindowSurface : Surface {
        static Material _guiTextureMat;
        public delegate void DrawToTargetFunc(Rect screenRect, Texture texture, Material mat);
        
        internal static Material _getGUITextureMat() {
            if (_guiTextureMat) {
                return _guiTextureMat;
            }

            var guiTextureShader = Shader.Find("UIWidgets/GUITexture");
            if (guiTextureShader == null) {
                throw new Exception("UIWidgets/GUITexture not found");
            }

            _guiTextureMat = new Material(guiTextureShader);
            return _guiTextureMat;
        }

        GrSurface _surface;
        private DrawToTargetFunc _drawToTargetFunc;
        public EditorWindowSurface(DrawToTargetFunc drawToTargetFunc = null)
        {
            this._drawToTargetFunc = drawToTargetFunc;
        }

        public SurfaceFrame acquireFrame(Size size, double devicePixelRatio) {
            this._createOrUpdateRenderTexture(size, devicePixelRatio);

            return new SurfaceFrame(this._surface,
                (frame, canvas) => this._presentSurface(canvas));
        }

        public void Dispose() {
            if (this._surface != null) {
                this._surface.Dispose();
                this._surface = null;
            }
        }

        protected bool _presentSurface(Canvas canvas) {
            if (canvas == null) {
                return false;
            }

            this._surface.getCanvas().flush();
            this._surface.getCanvas().reset();

            var screenRect = new Rect(0, 0,
                (float) (this._surface.size.width / this._surface.devicePixelRatio),
                (float) (this._surface.size.height / this._surface.devicePixelRatio));

            if (_drawToTargetFunc == null)
            {
                Graphics.DrawTexture(screenRect, this._surface.getRenderTexture(), _getGUITextureMat());
            }
            else
            {
                _drawToTargetFunc(screenRect, this._surface.getRenderTexture(), _getGUITextureMat());
            }
            return true;
        }

        void _createOrUpdateRenderTexture(Size size, double devicePixelRatio) {
            if (this._surface != null
                && this._surface.size == size
                && this._surface.devicePixelRatio == devicePixelRatio) {
                return;
            }

            if (this._surface != null) {
                this._surface.Dispose();
                this._surface = null;
            }

            this._surface = new GrSurface(size, devicePixelRatio);
        }
    }

    public class GrSurface : IDisposable {
        public readonly Size size;

        public readonly double devicePixelRatio;

        RenderTexture _renderTexture;

        Canvas _canvas;

        public RenderTexture getRenderTexture() {
            return this._renderTexture;
        }

        public Canvas getCanvas() {
            if (this._canvas == null) {
                this._canvas = new CommandBufferCanvas(this._renderTexture, (float) this.devicePixelRatio);
            }

            return this._canvas;
        }

        public GrSurface(Size size, double devicePixelRatio) {
            this.size = size;
            this.devicePixelRatio = devicePixelRatio;

            var desc = new RenderTextureDescriptor(
                (int) this.size.width, (int) this.size.height,
                RenderTextureFormat.Default, 24) {
                useMipMap = false,
                autoGenerateMips = false,
            };

            if (QualitySettings.antiAliasing != 0)
            {
                desc.msaaSamples = QualitySettings.antiAliasing;
            }

            this._renderTexture = new RenderTexture(desc);
        }

        public void Dispose() {
            D.assert(this._renderTexture);
            Object.DestroyImmediate(this._renderTexture);
            this._renderTexture = null;

            D.assert(this._canvas != null);
            this._canvas = null;
        }
    }
}