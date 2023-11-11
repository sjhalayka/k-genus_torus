#include "main.h"




float h1(const vertex_3& centre, const vertex_3& pos, const float r, const float R)
{
	float x = centre.x - pos.x;
	float y = centre.y - pos.y;
	float z = centre.z - pos.z;

	return powf(x * x + y * y + z * z + R * R - r * r, 2.0f) - 4.0f * R * R * (x * x + y * y);
}



int main(void)
{
	const float pi = 4.0f * atanf(1.0f);
	const float grid_max = 5.0;
	const float grid_min = -grid_max;
	const size_t res = 300;
	const bool make_border = true;
	const float isovalue = 1.0f; // bigger than zero
	const float border_value = 1.0f + isovalue;
	const int genus = 3;
	const float radius = 0.1f;
	const float outer_radius = 1.0f;

	const float circumference = (radius + outer_radius) * 2.0f * genus;
	const float kissing_radius = circumference / (2.0f * pi);


	vector<triangle> triangles;

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
			{
				const float step_angle = 2.0f * pi / genus;
				float angle = step_angle;

				for (size_t i = 0; i < genus; i++, angle += step_angle)
				{
					float x_angle = kissing_radius* cos(angle);
					float y_angle = kissing_radius* sin(angle);

					vertex_3 centre(x_angle, y_angle, 0);

					if (i == 0)
						xyplane0[x * res + y] = h1(centre, pos, radius, outer_radius);
					else
						xyplane0[x * res + y] *= h1(centre, pos, radius, outer_radius);
				}




			}
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
				{
					const float step_angle = 2.0f * pi / genus;
					float angle = step_angle;

					for (size_t i = 0; i < genus; i++, angle += step_angle)
					{
						float x_angle = kissing_radius*cos(angle);
						float y_angle = kissing_radius*sin(angle);

						vertex_3 centre(x_angle, y_angle, 0);

						if (i == 0)
							xyplane1[x * res + y] = h1(centre, pos, radius, outer_radius);
						else
							xyplane1[x * res + y] *= h1(centre, pos, radius, outer_radius);
					}


				}
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
