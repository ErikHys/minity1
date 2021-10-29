# Features implemented

* Task 1 textures
* Task 2 Object space normal mapping
  * Tangent space partially/wrongly implemented
  

# Where to find it
* model-base-fs.glsl
    * main
* model-base-gs.glsl
    * main
* Model.cpp and ModelRenderer.cpp
    * Reading and passing object and tangent maps
    * GUI
      * Can change mapping with the slider.
      * Turn on and of textures

# Bug
* Front lighting seems to be attached to camera somehow
    


# Results
### Head2 model
* Standard <br>
![](Head_standard.png)
* Object <br>
![](Head_object.png)
* Tangent <br>
![](Head_tangent.png)
### Head model
* Without textures standard <br>
![](without_textures_standard.png)
* With textures standard <br>
![](Textures_standard.png)
* Without textures object <br>
![](without_textures_object.png)
* With textures object <br>
![](textures_object.png)