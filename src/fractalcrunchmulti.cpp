/*
This file is part of geomandel. An artful fractal generator
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

#include "fractalcrunchmulti.h"

Fractalcrunchmulti::Fractalcrunchmulti(
    constants::fracbuff &buff, const std::shared_ptr<FractalParameters> &params)
    : Fractalcruncher(buff, params)
{
}

Fractalcrunchmulti::~Fractalcrunchmulti() {}
void Fractalcrunchmulti::fill_buffer()
{
    // a vector filled with futures. We will wait for all of them to be finished.
    std::vector<std::future<void>> futures;
    ctpl::thread_pool tpl(this->params->cores);

    // calculate the set line by line. Each line will be pushed to the
    // thread pool as separate job. The id parameter of the lambda function
    // represents the thread id.
    double x = this->params->x;
    double y = this->params->y;
    int iy = 0; /**< row to calculate*/
    for (auto &int_vec : buff) {
        futures.push_back(tpl.push([&int_vec, x, y, iy, this](int id) {
            double ypass = y;  // y value is constant for each row
            double xpass = x;
            if (iy != 0)
                ypass += this->params->ydelta * iy;
            for (unsigned int ix = 0; ix < this->params->xrange; ix++) {
                auto crunched_mandel =
                    this->crunch_complex(xpass, ypass, this->params->bailout);

                unsigned int its = std::get<0>(crunched_mandel);
                double Zx = std::get<1>(crunched_mandel);
                double Zy = std::get<2>(crunched_mandel);
                int_vec[ix] = this->iterations_factory(its, Zx, Zy);

                // increment xpass by xdelta
                xpass += this->params->xdelta;
            }
        }));
        iy++;
    }
    // make sure all jobs are finished
    for (const std::future<void> &f : futures) {
        f.wait();
    }
}
