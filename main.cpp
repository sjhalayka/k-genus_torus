#include "main.h"

float f(const float x, const int n)
{
	float result = 1.0;

	for(int i = 1; i <= n; i++)
	{
		const float i_float = static_cast<float>(i);

		result *= (result - (i_float - 1.0f)) * (result - i_float);
	}

	return result;
}

float g(const float x, const float y, const int n) 
{
	return powf(f(x, n), 1.0f) + powf(y, 2.0f);
}

float h(const float x, const float y, const float z, const int n, const float r, const float R)
{
	return powf(g(x, y, n), 1.0f) + powf(z, 2.0f) - powf(r, 2.0f);
}

float h1(const float x, const float y, const float z, const int n, const float r, const float R)
{
	return powf(x*x + y*y + z*z + R*R - r*r, 2.0f) - 4.0f * R*R * (x*x + y*y);
}



int main(void)
{
	const float grid_max = 5.0;
	const float grid_min = -grid_max;
	const size_t res = 100;
	const bool make_border = true;
	const float isovalue = 0.1f;
	const float border_value = 1.0f + isovalue;
	const int n = 2;
	const float radius = 0.1f;
	const float outer_radius = 1.0f;


	vector<triangle> triangles;
	vector<float> xyplane0(res*res, 0);
	vector<float> xyplane1(res*res, 0);

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
				xyplane0[x * res + y] = h1(pos.x, pos.y, pos.z, n, radius, outer_radius);
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
					xyplane1[x * res + y] = h1(pos.x, pos.y, pos.z, n, radius, outer_radius);
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
	cout << "Box counting dimension: " << logf(static_cast<float>(box_count)) / logf(1.0f/step_size) << endl;


	return 0;
}
