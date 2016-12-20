dom0Size = 2;

Point(1) = {0             , 0, 0,  2E-4 * dom0Size};
Point(2) = {0.5 * dom0Size, 0, 0, 50E-4 * dom0Size};
Point(3) = {      dom0Size, 0, 0,  1E-4 * dom0Size};

#Point(1) = {0             , 0, 0, 5E-10};
#Point(2) = {0.5 * dom0Size, 0, 0, 5E-10};
#Point(3) = {      dom0Size, 0, 0, 5E-10};


Line(1) = {1,2};
Line(2) = {2,3};

Physical Line(0) = {1,2};
