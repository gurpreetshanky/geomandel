/*
This file is part of geomandel. Mandelbrot Set infused by GeoTIFF
Copyright © 2015, 2016 Christian Rapp

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mandelcrunchsingle.h"

Mandelcrunchsingle::Mandelcrunchsingle(
    constants::fracbuff &buff, const std::shared_ptr<FractalParameters> &params)
    : Mandelcruncher(buff, params)
{
}

Mandelcrunchsingle::~Mandelcrunchsingle() {}
void Mandelcrunchsingle::fill_buffer()
{
    double x = this->params->x;
    double y = this->params->y;

    // calculate pixel by pixel
    for (unsigned int iy = 0; iy < this->params->yrange; iy++) {
        for (unsigned int ix = 0; ix < this->params->xrange; ix++) {
            auto crunched_mandel =
                this->crunch_mandel_complex(x, y, this->params->bailout);
            unsigned int its = std::get<0>(crunched_mandel);
            if (this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME ||
                this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME_2) {
                constants::Iterations it = this->iterations_factory(its, 0, 0);
                buff[iy][ix].default_index = it.default_index;
            }
            if (this->params->col_algo == constants::COL_ALGO::CONTINUOUS) {
                double Zx = std::get<1>(crunched_mandel);
                double Zy = std::get<2>(crunched_mandel);
                buff[iy][ix] = this->iterations_factory(its, Zx, Zy);
            }
            x += this->params->xdelta;
        }
        y += this->params->ydelta;
        x = this->params->xl;
    }
}
