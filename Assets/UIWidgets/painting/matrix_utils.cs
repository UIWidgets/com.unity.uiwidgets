﻿using System;
using UIWidgets.foundation;
using UIWidgets.ui;
using UnityEngine;
using Rect = UIWidgets.ui.Rect;

namespace UIWidgets.painting {
    public static class MatrixUtils {
        public static Offset transformPoint(Matrix4x4 transform, Offset point) {
            var position3 = new Vector3((float) point.dx, (float) point.dy, 0);
            var transformed3 = transform.MultiplyPoint(position3);
            return new Offset(transformed3.x, transformed3.y);
        }

        public static Rect transformRect(Matrix4x4 transform, Offset[] points, out bool isRect) {
            D.assert(points != null && points.Length == 4, "expected 4 points");

            var topLeft = MatrixUtils.transformPoint(transform, points[0]);
            var topRight = MatrixUtils.transformPoint(transform, points[1]);
            var bottomLeft = MatrixUtils.transformPoint(transform, points[2]);
            var bottomRight = MatrixUtils.transformPoint(transform, points[3]);

            isRect = topLeft.dy == topRight.dy
                     && topRight.dx == bottomRight.dx
                     && bottomRight.dy == bottomLeft.dy
                     && bottomLeft.dx == topLeft.dx;

            var left = Math.Min(Math.Min(Math.Min(topLeft.dx, topRight.dx), bottomLeft.dx), bottomRight.dx);
            var right = Math.Max(Math.Max(Math.Max(topLeft.dx, topRight.dx), bottomLeft.dx), bottomRight.dx);
            var top = Math.Min(Math.Min(Math.Min(topLeft.dy, topRight.dy), bottomLeft.dy), bottomRight.dy);
            var bottom = Math.Max(Math.Max(Math.Max(topLeft.dy, topRight.dy), bottomLeft.dy), bottomRight.dy);

            return Rect.fromLTRB(left, top, right, bottom);
        }

        public static Rect transformRect(Matrix4x4 transform, Rect rect, out bool isRect) {
            return MatrixUtils.transformRect(transform,
                new[] {rect.topLeft, rect.topRight, rect.bottomLeft, rect.bottomRight},
                out isRect);
        }

        public static Rect transformRect(Matrix4x4 transform, Offset[] points) {
            bool isRect;
            return MatrixUtils.transformRect(transform, points, out isRect);
        }

        public static Rect transformRect(Matrix4x4 transform, Rect rect) {
            bool isRect;
            return MatrixUtils.transformRect(transform, rect, out isRect);
        }

        public static Rect inverseTransformRect(Matrix4x4 transform, Rect rect) {
            D.assert(rect != null);
            D.assert(transform.determinant != 0.0);

            if (transform.isIdentity) {
                return rect;
            }

            transform = transform.inverse;
            return MatrixUtils.transformRect(transform, rect);
        }
        
        public static Offset getAsTranslation(ref Matrix4x4 transform) {
            if (transform.m00 == 1.0 &&
                transform.m10 == 0.0 &&
                transform.m20 == 0.0 &&
                transform.m30 == 0.0 &&
                transform.m01 == 0.0 &&
                transform.m11 == 1.0 &&
                transform.m21 == 0.0 &&
                transform.m31 == 0.0 &&
                transform.m02 == 0.0 &&
                transform.m12 == 0.0 &&
                transform.m22 == 1.0 &&
                transform.m32 == 0.0 &&
                transform.m23 == 0.0 &&
                transform.m33 == 1.0) {
                return new Offset(transform.m03, transform.m13);
            }
            return null;
        }
    }
}