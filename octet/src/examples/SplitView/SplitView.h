////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
	/// Scene containing a box with octet.
	class SplitView : public app {
		// scene for drawing box
		ref<visual_scene> app_scene;
		ref<camera_instance> current_camera;
		dynarray<vec3p> grid_vertices;

	public:
		SplitView(int argc, char **argv) : app(argc, argv) {
		}

		~SplitView() {
		}

		/// this is called once OpenGL is initialized
		void app_init() {
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 0));

			app_scene->create_camera();
			app_scene->get_camera_instance(1)->get_node()->rotate(-45, vec3(1, 0, 0));
			app_scene->get_camera_instance(1)->get_node()->translate(vec3(0, 25, 0));



			material *red = new material(vec4(1, 0, 0, 1));
			material *green = new material(vec4(0, 1, 0, 1));
			material *blue = new material(vec4(0, 0, 1, 1));

			mat4t mat;
			mat.translate(-3, 6, 0);
			app_scene->add_shape(mat, new mesh_sphere(vec3(2, 2, 2), 2), red, true);

			mat.loadIdentity();
			mat.translate(0, 10, 0);
			app_scene->add_shape(mat, new mesh_box(vec3(2, 2, 2)), red, true);

			mat.loadIdentity();
			mat.translate(3, 6, 0);
			app_scene->add_shape(mat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), blue, true);

			// ground
			mat.loadIdentity();
			mat.translate(0, -1, 0);
			app_scene->add_shape(mat, new mesh_box(vec3(200, 0, 200)), green, false);


			app_scene->get_mesh_instance(0)->get_mesh()->make_wireframe();
			app_scene->get_mesh_instance(1)->get_mesh()->make_wireframe();
			app_scene->get_mesh_instance(2)->get_mesh()->make_wireframe();

		}

		void make_grid(float dx, float dy, int max) {
			float i = 0.0, j = 0.0;
			vec3p pos;
			for (i = -max; i < max; i = i + dx) {
				for (j = -max; j < max; j = j + dy) {
					pos=vec3p(i, 0, j);
					grid_vertices.push_back(pos);
				}
			}
		}//make_grid


		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			int vx = 1, vy = 1;

      //view1
			//split window into 2 views side by side
			set_viewport_size(w , h/2);
			get_viewport_size(vx, vy);
				
			app_scene->begin_render_view(0,0, vx, vy );
			

			current_camera = app_scene->get_camera_instance(0);

			app_scene->render_view((float)vx / vy, current_camera);

	//view2
			//no need as the new views are the same size
			//set_viewport_size(w /4 , h/4);
		    get_viewport_size(vx, vy);

			
			//begin render using new viewports
			    // I modified begin_render() so we can reposition viewport in another part of the window
			app_scene->begin_render_view(0, vy, vx, vy);

			//change camera
			current_camera = app_scene->get_camera_instance(1);

			// render using view of the current camera
			app_scene->render_view( (float)vx/vy, current_camera);

  //update whole scene
			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);
		}
	};
}
