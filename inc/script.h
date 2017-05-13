#ifndef SCRIPT_H
#define SCRIPT_H

#include "global.h"
#include <vector>
#include <algorithm>

class GenericTween {
	public:
		virtual ~GenericTween(){}
		virtual bool step(){ return false; }
};

template <class T> class Tween : public GenericTween {
	public:
		/* yeah this breaks encapsulation sue me */
		Tween<T>(T *var, T init, T fin, int frames) : var(var), fin(fin), frames(frames){
			*var = init;
			amount = (fin - *var) / frames;
			current_step = 0;
		}
		virtual ~Tween<T>() {}
		virtual bool step() {
			if (current_step < frames) {
				*var += amount;
				current_step++;
				return true;
			}
			else {
				*var = fin;
				return false;
			}
		}

	protected:
		T *var;
		T fin;
		T amount;

		int frames;
		int current_step;
};

template <class T> class LoopTween : public GenericTween {
	public:
		LoopTween<T>(T *var, T init, T fin, int frames, int repeats=0, bool bounce=true) : 
			var(var),
			init(init),
			fin(fin),
			frames(frames),
			repeats(repeats),
			bounce(bounce) 
		{
				amount = (fin - *var) / frames;
				*var = init;
				current_step = 0;
				forwards=true;
		}
		virtual ~LoopTween<T>() {}
		virtual bool step() {
			if (current_step < frames) {
				*var += (forwards ? amount : -amount);
				current_step++;
				return true;
			}
			else {
				if (--repeats == 0) {
					*var = init;
					return false;
				}
				else {
					if (bounce)
						forwards = !forwards;
					else
						*var = init;
					current_step = 0;
					return true;
				}
			}
		}
			
	protected:
		T *var;
		T init;
		T fin;
		T amount;
		int frames;
		int repeats;
		bool bounce;
		bool forwards;
/*		std::vector<T> steps; */
		int current_step;
};

			

/*
template <class T> class CallbackTween : public Tween {
	public:
		CallbackTween<T>(T *var, T init, T fin, int frames, T threshold, void (*callback)()) : Tween(var, init, fin, frames, threshold), callback(callback) {}
		~CallbackTween<T>() {}

		bool step() {
			bool res = super::step();

			if (!res) {
				(*callback)();
			}

			return res;
		}

	protected:
		typedef Tween super;
		void (*callback)();
};
*/

template <class T> class BounceTween : public GenericTween {
	public:
		BounceTween<T> (T *var, T init, T fin, int frames, T threshold) : var(var), init(init), fin(fin), frames(frames), threshold(threshold) {
			*var = init;
			amount = (fin - *var) / frames;
			direction = true;
		}
		~BounceTween<T>() {}
		bool step() {
			T diff;
			if (direction) {
				diff = MAX(fin, *var) - MIN(fin, *var);
				if (diff > threshold) {
					*var += amount;
				}
				else {
					*var = fin;
					direction = !direction;
					*var -= amount;
				}
			}
			else {
				diff = MAX(init, *var) - MIN(init, *var);
				if (diff > threshold) {
					*var -= amount;
				}
				else {
					*var = init;
					direction = !direction;
					*var += amount;
				}
			}

			return true;
		}
	private:
		T *var;
		T init;
		T fin;
		T amount;

		int frames;
		T threshold;
		int timer;

		bool direction;

};


/* random tween for shakes */
/* LoopTween with stop function, pingpong, norepeatends, loop points separate from starting value, etc */

#endif
