struct timespec timespec_normalized(time_t sec, long nsec);

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);

struct timespec timespec_add(struct timespec lhs, struct timespec rhs);

int timespec_cmp(struct timespec lhs, struct timespec rhs);

/* Busy wait using clock_gettime() */
void busy_wait(struct timespec t);

/* Busy wait using times() */
int busy_wait_times();

/* Busy wait using sleep() */
int wait_sleep();



void main ();
