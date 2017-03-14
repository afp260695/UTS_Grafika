#ifndef TARGET_HPP
#define TARGET_HPP

class Target
{
public:
	Target();
	~Target();

private:
	Color targetColor;
	bool isTargetShot;

	int kepalaX;
	int kepalaY;
	int kecX;
	int kecY;
	int targetDir;
	int targetUjung;
	bool stop;

	int targetX;
	int targetY;
	bool targetAlive;
	
};

#endif