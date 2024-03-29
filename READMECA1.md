# Features implemented

 * Blinn-Phong illumination, with corresponding uniforms passed to shaders
 * Extended GUI to include, in models dropdown
   * Slider for shininess
     * The power the Blinn Phong value is raised to
   * Sliders for light intensity
     * For front(key)
     * Side(fill)
     * Back
   * Slider for light color
   * Checkbox for cel shading
   * Slider for how many levels of cel shading
     * Meaning how many parts the shading would be divided into
   * Cel Shading
   * 3 point lighting
 
# Where to find it
   * ModelRenderer.cpp:
     * 81-87 Init of new variables to be sent to shaders
     * 95-101 GUI features
     * 134-140 set uniforms for shader
   * model-base-fs.glsl
     * getAmbientColor() -> Returns ambient lighting for fragment
     * getDiffuse() -> Returns Diffuse lighting
     * getBlinnPhong() -> does Blinn Phong illumination
     * getCelShading() -> returns a float between 0.0 - 1.0 for cel shading
     * getShading() -> Does the shading combined, not including Blinn Phong
     * main() -> combines key, fill and backlight, and cel shading if necessary

# Models

* added a ball, which was slightly uneven

# Extra
* Cel shading
  * First I get the angle with the dot product between the light and fragment normal
  * Then I multiply the angle with the number of levels I want in the cel shading, where the levels are how many "layers/levels"
  of shade I want, and I take the ceiling of that.
  * Then I add a black line if the angle*levels modulo 1 is < 0.04 or > 0.96. this is to create a border between each level.
* Three point light
  * I do the same shading as usual but add a backlight opposite the world camera, -worldCameraPosition. Then a fill light
  as a reflect of - worldLightPosition, -worldCameraPosition


# Results

## Skull model only Blinn Phong front(key) light <br>
![](Skull-bp-standard.png)

## Skull model only Blinn Phong only fill light<br>
![](Skull-bp-onlyfill.png)

## Skull model only Blinn Phong only back light<br>
![](Skull-bp-onlyback.png)

## Skull model three point light<br>
![](Skull-threepoint.png)

## Skull model key light pink<br>
![](Skull-bp-top-pink.png)

## Skull model top light<br>
![](Skull-bp-toplight.png)

## Cel shading on ball pink<br>
![](cel-ball-pink.png)

## Cel shading on side ball pink<br>
![](cel-ball-pink-side.png)

## GUI
![](gui.png)