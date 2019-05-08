# model 2 depth


aspect = height / width;
viewHorizontal = right * 2 * tan( PI * fov / 360);
viewVertical = up * 2 * tan( aspect * PI * fov / 360);
viewDepth = CameraDirection;

//2d r_y is pix y is distance
y = r_y * c
c = f * tan(fov / 2) / (viewVertical / 2)
//3d
Y = Z * y / f = Z * r_y * tan(fov / 2) / (viewVertical / 2)
# depth 2 cloud

# cloud 变换
https://blog.csdn.net/weixin_38275649/article/details/80968364

```vs
	imgFo << "M";
	auto cam = thisDepthTracer->GetScene()->pCamera;
	imgFo << " " << cam.right.x << " " << cam.right.y << " " << cam.right.z;//X
	imgFo << " " << -cam.up.x << " " << -cam.up.y << " " << -cam.up.z;//-Y
	imgFo << " " << cam.CameraDirection.x << " " << cam.CameraDirection.y << " " << cam.CameraDirection.z << endl;//Z
	imgFo << "P" << " " << cam.CameraPosition.x * camera_factor << " " << cam.CameraPosition.y * camera_factor << " " << cam.CameraPosition.z * camera_factor << endl;//position
```

误差比较大

# bilterail filtter
模糊 了深度图