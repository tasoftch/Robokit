//
// Created by Thomas Abplanalp on 25.03.25.
//

#include "private/filter.h"

typedef struct {
	uint8_t mode;
	uint8_t v_th;
	uint8_t t_th;
	uint8_t trust;
	int16_t old_angle;
	int16_t ref_angle;
} S_filter_angular;

typedef enum {
	S_FILTER_MODE_ANGULAR = 1<<0,
	S_FILTER_MODE_ANGULAR_REFERENCED = 1<<1,
} S_filter_mode;


#define _ROBOKIT_FILTER_CAST(TYPE, FILTER) ( (TYPE) (FILTER) )


void filter_init(S_filter *filter) {
	if(filter) {
		*filter = (S_filter){0};
	}
}

void filter_angle_init(S_filter *filter, uint8_t deviation_threshold, uint8_t trust_threshold) {
	filter_init(filter);
	_ROBOKIT_FILTER_CAST(S_filter_angular *, filter)->mode = 1;
	_ROBOKIT_FILTER_CAST(S_filter_angular*, filter)->v_th = deviation_threshold;
	_ROBOKIT_FILTER_CAST(S_filter_angular*, filter)->t_th = trust_threshold;
}

void filter_angle_put_reference(S_filter *filter) {
	if (filter && filter->mode & S_FILTER_MODE_ANGULAR) {
		_ROBOKIT_FILTER_CAST(S_filter_angular*, filter)->ref_angle =
			_ROBOKIT_FILTER_CAST(S_filter_angular *, filter)->old_angle;
		_ROBOKIT_FILTER_CAST(S_filter_angular*, filter)->mode |= S_FILTER_MODE_ANGULAR_REFERENCED;
	}
}

void filter_angle_dereference(S_filter *filter) {
	if (filter && filter->mode & S_FILTER_MODE_ANGULAR) {
		_ROBOKIT_FILTER_CAST(S_filter_angular*, filter)->mode &= ~ S_FILTER_MODE_ANGULAR_REFERENCED;
	}
}

void filter_angle_put_value(S_filter *filter, int16_t angle) {
	if (filter && filter->mode & S_FILTER_MODE_ANGULAR) {
		S_filter_angular *f = (S_filter_angular *) filter;

		if (f->mode & S_FILTER_MODE_ANGULAR_REFERENCED) {
			angle = (angle - f->ref_angle + 2880) % 5760;
			if (angle < 0) {
				angle += 5760;
			}
			angle -= 2880;
		}

		int16_t diff = f->old_angle - angle;
		if ((diff < 0 ? -diff : diff) < f->v_th) {
			f->old_angle = angle;
			f->trust = 0;
		} else {
			f->trust++;
			if(f->trust >= f->t_th) {
				f->old_angle = angle;
				f->trust = 0;
			}
		}
	}
}

int16_t filter_angle_get_value(S_filter *filter) {
	if (filter && filter->mode & S_FILTER_MODE_ANGULAR) {
		return _ROBOKIT_FILTER_CAST(S_filter_angular*, filter)->old_angle;
	}
	return 0;
}