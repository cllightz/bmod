//コンソール上での入力

int getInt(void){
	char buffer[256]={0};
	int val;
	while( fgets(buffer, sizeof(buffer), stdin) != NULL ) {
		if( sscanf(buffer, "%d", &val) == 1) {
			return val;
		}
	}
}