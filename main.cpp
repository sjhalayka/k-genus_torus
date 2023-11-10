#include "main.h"




void create_torus(vector<triangle> &triangles)
{
	triangles.clear();

	const float pi = 4.0f * atanf(1.0f);

	float major_radius = 1.0f;
	float minor_radius = 0.3f;

	float genus_radius = 0.2f; // radius for the genus

	int slices = 100;
	int stacks = 100;

	for (int i = 0; i < slices; ++i)
	{
		float theta1 = i * 2.0f * pi / slices;
		float theta2 = (i + 1) * 2.0f * pi / slices;

		for (int j = 0; j < stacks; ++j)
		{
			float phi1 = j * 2.0f * pi / stacks;
			float phi2 = (j + 1.0) * 2.0f * pi / stacks;

			vertex_3 p1((major_radius + minor_radius * cos(theta1)) * cos(phi1),
				(major_radius + minor_radius * cos(theta1)) * sin(phi1),
				minor_radius * sin(theta1));

			vertex_3 p2((major_radius + minor_radius * cos(theta2)) * cos(phi1),
				(major_radius + minor_radius * cos(theta2)) * sin(phi1),
				minor_radius * sin(theta2));

			vertex_3 p3((major_radius + minor_radius * cos(theta2)) * cos(phi2),
				(major_radius + minor_radius * cos(theta2)) * sin(phi2),
				minor_radius * sin(theta2));

			vertex_3 p4((major_radius + minor_radius * cos(theta1)) * cos(phi2),
				(major_radius + minor_radius * cos(theta1)) * sin(phi2),
				minor_radius * sin(theta1));

			triangle a;

			a.vertex[0] = p3;
			a.vertex[1] = p2;
			a.vertex[2] = p1;
			triangles.push_back(a);
			//torus.make_triangle(p1, p2, p3);
			
			a.vertex[0] = p4;
			a.vertex[1] = p3;
			a.vertex[2] = p1;
			triangles.push_back(a);		
	//		torus.make_triangle(p1, p3, p4);

			// Extra loops for the genus
			for (int k = 0; k < 100; ++k)
			{
				float psi1 = k * 2.0f * pi / 5.0;
				float psi2 = (k + 1.0f) * 2.0f * pi / 5.0;

				vertex_3 p5((major_radius + genus_radius * cos(psi1)) * cos(phi1),
					(major_radius + genus_radius * cos(psi1)) * sin(phi1),
					genus_radius * sin(psi1));

				vertex_3 p6((major_radius + genus_radius * cos(psi2)) * cos(phi1),
					(major_radius + genus_radius * cos(psi2)) * sin(phi1),
					genus_radius * sin(psi2));

				vertex_3 p7((major_radius + genus_radius * cos(psi2)) * cos(phi2),
					(major_radius + genus_radius * cos(psi2)) * sin(phi2),
					genus_radius * sin(psi2));

				vertex_3 p8((major_radius + genus_radius * cos(psi1)) * cos(phi2),
					(major_radius + genus_radius * cos(psi1)) * sin(phi2),
					genus_radius * sin(psi1));

				
				
				
				a.vertex[0] = p7;
				a.vertex[1] = p6;
				a.vertex[2] = p5;
				triangles.push_back(a);
				//torus.make_triangle(p5, p6, p7);
				
				
				
				a.vertex[0] = p8;
				a.vertex[1] = p7;
				a.vertex[2] = p5;
				triangles.push_back(a);
				//torus.make_triangle(p5, p7, p8);
			}
		}
	}
}






















float f(const float x, const int n)
{
	float result = 1.0;

	for (int i = 1; i <= n; i++)
	{
		const float i_float = static_cast<float>(i);

		result *= (result - (i_float - 1.0f)) * (result - i_float);
	}

	return result;
}

float h(const float x, const float y, const float z, const int n, const float r, const float R)
{
	return f(x, n) + powf(y, 2.0f) + powf(z, 2.0) - powf(r, 2.0);
}







float h1(const float x, const float y, const float z, const int n, const float r, const float R)
{
	return powf(x * x + y * y + z * z + R * R - r * r, 2.0f) - 4.0f * R * R * (x * x + y * y);
}

float h2(const float x, const float y, const float z, const int n, const float r, const float R)
{
	return powf(R - sqrt(x * x + y * y), 2.0f) + z * z - r * r;
}

float h3(const float x, const float y, const float z, const int n, const float r, const float R)
{
	return powf(x * x + y * y + z * z + r * r - R * R, 3.0f) - 4.0f * R * R * (x * x + y * y) - 4.0f * (z * z);
}




int main(void)
{


	


	const float grid_max = 5.0;
	const float grid_min = -grid_max;
	const size_t res = 100;
	const bool make_border = true;
	const float isovalue = 0.001f;
	const float border_value = 1.0f + isovalue;
	const int n = 1;
	const float radius = 0.1f;
	const float outer_radius = 1.0f;


	vector<triangle> triangles;


	create_torus(triangles);


	if (0 < triangles.size())
		write_triangles_to_binary_stereo_lithography_file(triangles, "out.stl");

	return 0;









	vector<float> xyplane0(res * res, 0);
	vector<float> xyplane1(res * res, 0);

	const float step_size = (grid_max - grid_min) / (res - 1);

	size_t z = 0;

	vertex_3 pos(grid_min, grid_min, grid_min);

	// Calculate xy plane 0.
	for (size_t x = 0; x < res; x++, pos.x += step_size)
	{
		pos.y = grid_min;

		for (size_t y = 0; y < res; y++, pos.y += step_size)
		{
			if (true == make_border && (x == 0 || y == 0 || z == 0 || x == res - 1 || y == res - 1 || z == res - 1))
				xyplane0[x * res + y] = border_value;
			else
				xyplane0[x * res + y] = h(pos.x, pos.y, pos.z, n, radius, outer_radius);
		}
	}

	// Prepare for xy plane 1.
	z++;
	pos.z += step_size;

	size_t box_count = 0;

	// Calculate xy planes 1 and greater.
	for (; z < res; z++, pos.z += step_size)
	{
		pos.x = grid_min;

		cout << "Calculating triangles from xy-plane pair " << z << " of " << res - 1 << endl;

		for (size_t x = 0; x < res; x++, pos.x += step_size)
		{
			pos.y = grid_min;

			for (size_t y = 0; y < res; y++, pos.y += step_size)
			{
				if (true == make_border && (x == 0 || y == 0 || z == 0 || x == res - 1 || y == res - 1 || z == res - 1))
					xyplane1[x * res + y] = border_value;
				else
					xyplane1[x * res + y] = h(pos.x, pos.y, pos.z, n, radius, outer_radius);
			}
		}

		tesselate_adjacent_xy_plane_pair(
			box_count,
			xyplane0, xyplane1,
			z - 1,
			triangles,
			isovalue,
			grid_min, grid_max, res,
			grid_min, grid_max, res,
			grid_min, grid_max, res);

		xyplane1.swap(xyplane0);
	}

	cout << endl;

	if (0 < triangles.size())
		write_triangles_to_binary_stereo_lithography_file(triangles, "out.stl");

	// Print box-counting dimension
	// Make sure that step_size != 1.0f :)
	cout << "Box counting dimension: " << logf(static_cast<float>(box_count)) / logf(1.0f / step_size) << endl;


	return 0;
}
