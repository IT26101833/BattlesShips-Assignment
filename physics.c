double minimum_velocity(){



}


double maximum_velocity(){


}

double teta_rad(double angle){

	return angle * (PI/180); 
}

double max_angle(){


}

double range(double speed, double teta){
	return (pow(speed, 2)* sin(2 * teta))/GRAVITY;
}