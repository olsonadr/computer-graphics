#include "../include/osu/osusphere.hpp"

inline
struct osuSpherePoint *
PtsPointer( int lat, int lng )
{
	if( lat < 0 )	lat += (NumLats-1);
	if( lng < 0 )	lng += (NumLngs-0);
	if( lat > NumLats-1 )	lat -= (NumLats-1);
	if( lng > NumLngs-1 )	lng -= (NumLngs-0);
	return &Pts[ NumLngs*lat + lng ];
}

inline
void
DrawPoint( struct osuSpherePoint *p )
{
	glNormal3fv( &p->nx );
	glTexCoord2fv( &p->s );
	glVertex3fv( &p->x );
}

inline
void
DrawPoint(struct osuSpherePoint *p, float r, float g, float b)
{
    glColor3f(r, g, b);
    glNormal3fv(&p->nx);
    glTexCoord2fv(&p->s);
    glVertex3fv(&p->x);
}

void
OsuSphere( float radius, int slices, int stacks, float r, float g, float b )
{
	// set the globals
	NumLngs = slices;
	NumLats = stacks;
	if( NumLngs < 3 )
		NumLngs = 3;
	if( NumLats < 3 )
		NumLats = 3;

	// allocate the osuSpherePoint data structure
	Pts = new struct osuSpherePoint[ NumLngs * NumLats ];

    float min_s = 2, max_s = -1;
    float min_t = 2, max_t = -1;

	// fill the Pts structure
	for( int ilat = 0; ilat < NumLats; ilat++ ) {
		float lat = -M_PI/2.  +  M_PI * (float)ilat / (float)(NumLats-1);	// ilat=0/lat=0. is the south pole
											                                // ilat=NumLats-1, lat=+M_PI/2. is the north pole
		float xz = cosf( lat );
		float y  = sinf( lat );
		for( int ilng = 0; ilng < NumLngs; ilng++ )	    // ilng=0, lng=-M_PI and
											            // ilng=NumLngs-1, lng=+M_PI are the same meridian
		{
			float lng = -M_PI  +  2. * M_PI * (float)ilng / (float)(NumLngs-1);
			float x =  xz * cosf( lng );
			float z = -xz * sinf( lng );
			struct osuSpherePoint* p = PtsPointer( ilat, ilng );
			p->x  = radius * x;
			p->y  = radius * y;
			p->z  = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;
			p->s = (lng+M_PI) / (2.*M_PI);
			p->t = (lat+M_PI/2.) / M_PI;

            if (p->s < min_s) min_s = p->s;
            if (p->s > max_s) max_s = p->s;
            if (p->t < min_t) min_t = p->t;
            if (p->t > max_t) max_t = p->t;
		}
    }
    // printf("min_max_s=(%.3f, %.3f); min_max_t=(%.3f, %.3f); \n", min_s, max_s, min_t, max_t);

	struct osuSpherePoint top, bot;		// top, bottom points

	top.x =  0.;		top.y  = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s  = 0.;		top.t  = 1.;

	bot.x =  0.;		bot.y  = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s  = 0.;		bot.t  =  0.;

    // establish color
    // glColor3f(r, g, b);
    // glColor3f(1., 1., 1.);


    // connect the north pole to the latitude NumLats-2
    glBegin(GL_TRIANGLE_STRIP);
    for (int ilng = 0; ilng < NumLngs; ilng++)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
		top.s = (lng + M_PI) / (2. * M_PI);
		DrawPoint(&top);
		struct osuSpherePoint* p = PtsPointer(NumLats - 2, ilng);	// ilat=NumLats-1 is the north pole
		DrawPoint(p);
	}
	glEnd();

	// connect the south pole to the latitude 1
	glBegin( GL_TRIANGLE_STRIP );
	for (int ilng = NumLngs - 1; ilng >= 0; ilng--)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
		bot.s = (lng + M_PI) / (2. * M_PI);
        DrawPoint(&bot);
        struct osuSpherePoint* p = PtsPointer(1, ilng);					// ilat=0 is the south pole
        DrawPoint(p);
    }
	glEnd();

	// connect the horizontal strips
	for( int ilat = 2; ilat < NumLats-1; ilat++ )
	{
		struct osuSpherePoint* p;
		glBegin(GL_TRIANGLE_STRIP);
		for( int ilng = 0; ilng < NumLngs; ilng++ )
		{
			p = PtsPointer( ilat, ilng );
            DrawPoint(p);
            p = PtsPointer( ilat-1, ilng );
            DrawPoint(p);
        }
		glEnd();
	}

	// clean-up
	delete [ ] Pts;
	Pts = NULL;
}

void OsuSphere(float radius, int slices, int stacks)
{
    OsuSphere(radius, slices, stacks, 1., 1., 1.);
}
