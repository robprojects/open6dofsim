#include "render.h"
//#include "motion.h"
#include "geo6dof.h"
#include <QOpenGLFunctions>

Render::Render(QWidget* parent)
	: QOpenGLWidget(parent) {

	
}

Render::~Render() {

}

void Render::updateDisplay(struct Geo6Dof::display_pose *pose) {
	m_pose = pose;
	update();
}

static const char* vertexShaderSource =
"#version 330\n"
"uniform mat4 mvpMatrix;\n"
"in vec4 vertex;\n"
"in vec4 color;\n"
"out vec4 varyingColor;\n"
"void main(){\n"
" varyingColor = color;\n"
" gl_Position = mvpMatrix * vertex;\n"
"}\n";

static const char* fragmentShaderSource =
"#version 330\n"
"in vec4 varyingColor;\n"
"out vec4 fragColor;\n"
"void main(){\n"
" fragColor = varyingColor;\n"
"}\n";




void Render::initializeGL() {
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	f->glEnable(GL_DEPTH_TEST);
	f->glEnable(GL_CULL_FACE);

	f->glClearColor(0.0, 0.0, 0.4, 0.0);

	m_program = new QOpenGLShaderProgram;
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

	m_program->link();
}

#if 0
void bargraph(int act) {
	// bar

	int width = 150, height = 20, top = (480 - act * (height + 5));
	int bwidth = 150 - ((int)(act_state[act].actual_pos * 150) / (1 << 16));
	int cwidth = 150 - ((int)(act_state[act].cmd_pos * 150) / (1 << 16));
	struct timeval now, diff;

	servo_state_t sstate = FLAGS_STATE(act_state[act].last_flags);

	gettimeofday(&now, NULL);

	float r = 0.0f, g = 0.0f, b = 0.0f;

	switch (sstate) {
	case SERVO_INIT:
	case SERVO_FINDHOME:
		b = 1.0f;
		break;
	case SERVO_RUN:
		g = 1.0f;
		break;
	case SERVO_ERROR:
		r = 1.0f;
		break;
	case SERVO_STOP:
	case SERVO_STOP_NOHOME:
		r = 1.0f; g = 1.0f;
		break;
	}

	if (act_state[act].active) {
		timersub(&now, &(act_state[act].last_response), &diff);
		float dim = (1000000 - diff.tv_usec) / 1000000.0;
		r = r *= dim;
		g = g *= dim;
		b = b *= dim;
		if (diff.tv_sec != 0) { r = 1.0f; g = 0.0f; b = 0.0f; }
		glBegin(GL_QUADS);
		glColor3f(r, g, b); // red
		glVertex2f(0, top);
		glVertex2f(0 + bwidth, top);
		glVertex2f(0 + bwidth, top - height);
		glVertex2f(0, top - height);
		glVertex2f(0, top);
		glEnd();
	}

	// outline
	glBegin(GL_LINES);
	if (act_state[act].active) {
		glColor3f(1.0f, 1.0f, 1.0f); // white
		if (act_state[act].last_flags & FLAG_BRAKE_LIMIT) {
			glColor3f(1.0f, 0.0f, 0.0f); // red 
		}
		else {
			glColor3f(1.0f, 1.0f, 1.0f); // white
		}
	}
	else {
		glColor3f(0.5f, 0.5f, 0.5f); // gray
	}
	glVertex2f(0, top);
	glVertex2f(0 + width, top);
	glVertex2f(0 + width, top);
	glVertex2f(0 + width, top - height);
	glVertex2f(0 + width, top - height);
	glVertex2f(0, top - height);
	glVertex2f(0, top - height);
	glVertex2f(0, top);
	glEnd();

	// cmd pos line
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f); // white
	glVertex2f(0 + cwidth, top);
	glVertex2f(0 + cwidth, top - height);
	glEnd();
}
#endif

#if 0
void display(void) {
	int i;

	glEnable(GL_DEPTH_TEST);
	//gluPerspective(45.0f, aspect, 0.1f, 100.0f);
	//glMatrixMode(GL_PROJECTION);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, 0.5f);
	//glRotatef(10.0f, 1.0f, 0.0f, 1.0f);
	gluLookAt(0.0f, 1.0f, 3.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
	//gluLookAt(0.0f,1.5f,5.0f,0.0f,1.3f,4.0f,0.0f,1.0f,0.0f);
	//glTranslatef(0.0f, 0.0f, -7.0f);

	//glMatrixMode(GL_MODELVIEW);


//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
	//glEnable(GL_DEPTH_TEST);

//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluPerspective(45.0f, aspect, 0.1f, 100.0f);

	//glTranslatef(0.0f, 0.0f, -7.0f);
	  //glRotatef(80.0f, 1.0f, 0.0f, 0.0f);
	 //gluLookAt(0.0f, 1.5f/* + z*/, 5.0f,
//		0.0f, 1.3f, 4.0f,
	//	0.0f, 1.0f, 0.0f);

	// base
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	for (i = 0; i < 6; i++) {
		glVertex3f(geo.base[i][0], geo.base[i][2], geo.base[i][1]);
	}
	glEnd();

	// actuators
	glLineWidth(3.0);
	for (i = 0; i < N; i++) {
		glColor3f((i == hilight) ? 0.0 : 1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(geo.b[i][0], geo.b[i][2], geo.b[i][1]);
		glVertex3f(st.p[i][0], st.p[i][2], st.p[i][1]);
		glEnd();
	}

	// platform
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	for (i = 0; i < 6; i++) {
		glVertex3f(st.platform[i][0], st.platform[i][2], st.platform[i][1]);
	}
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glDisable(GL_DEPTH_TEST);
	glLoadIdentity();
	gluOrtho2D(0, 640, 0, 480);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// graph

	glLineWidth(1.0);

	float graph_min = FLT_MAX;
	float graph_max = -FLT_MAX;
	for (i = 0; i < (hilight < 6 ? 2 : NUM_GRAPH); i++) {
		int j = 0;
		for (j = 0; j < 640; j++) {
			if (graph[i][j] < graph_min) graph_min = graph[i][j];
			if (graph[i][j] > graph_max) graph_max = graph[i][j];
		}
	}
	//printf("min %f, max %f\n", graph_min, graph_max); 
	float scale = 200.0 / ((float)(graph_max - graph_min));

	for (i = 0; i < NUM_GRAPH; i++) {
		switch (i) {
		case 0:
			glColor3f(0.5, 0.0, 0.0); break;
		case 1:
			glColor3f(0.0, 0.5, 0.0); break;
		case 2:
			glColor3f(0.0, 0.0, 0.5); break;
		}
		int j;
		float prev_point = graph[i][graph_pos];
		for (j = 1; j < 640; j++) {
			float point = graph[i][(j + graph_pos) % 640];
			glBegin(GL_LINES);
			glVertex2f(j, (float)(prev_point - graph_min) * scale);
			glVertex2f(j + 1, (float)(point - graph_min) * scale);
			glEnd();
			prev_point = point;
		}
	}

	glLineWidth(3.0);

	// bar graph
	for (i = 0; i < 6; i++) {
		bargraph(i);
	}

	if (stopped || stop_height < 500) {
		char stoptext[100];
		if (!stopped) {
			sprintf(stoptext, "STARTING IN %d/500", stop_height);
		}
		else {
			sprintf(stoptext, "STOPPED - 'T' TO TOGGLE %d", hilight);
		}
		glColor3f(1.0, 1.0, 1.0);
		char* c = stoptext;
		int pos = 320 - (strlen(stoptext) * 9) / 2;
		for (; *c != '\0'; c++) {
			glRasterPos2f(pos, 240);
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
			pos += 9;
		}
	}
	char gtext[100];
	switch (hilight) {

	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		sprintf(gtext, "ACTUATOR %d", hilight);
		break;
	case 6:
		sprintf(gtext, "pos.theta/psi/phi");
		break;
	case 7:
		sprintf(gtext, "T[3]");
		break;
	}
	glColor3f(1.0, 1.0, 1.0);
	char* c = gtext;
	int pos = 320 - (strlen(gtext) * 9) / 2;
	for (; *c != '\0'; c++) {
		glRasterPos2f(pos, 50);
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
		pos += 9;
	}


	glFlush();
	glutSwapBuffers();
}
#endif

void Render::paintGL() {

	if (!m_pose) return;

	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_program->bind();

	m_program->setUniformValue("mvpMatrix", m_projection * m_view);
	m_program->setUniformValue("color", QColor(Qt::white));

	QVector4D colors[6*4];
	QVector3D vertices[6*4];
	int i;
	// base
	for (i = 0; i < 6; i++) {
		//glVertex3f(geo.base[i][0], geo.base[i][2], geo.base[i][1]);
		vertices[i] = m_pose->base[i];
		colors[i] = QVector4D(1.0, 0.0, 0.0, 0.0); // red
	}
	// platform
	for (i = 0; i < 6; i++) {
		//glVertex3f(st.platform[i][0], st.platform[i][2], st.platform[i][1]);
		vertices[i + 6] = m_pose->platform[i];
		colors[i + 6] = QVector4D(0.0, 1.0, 0.0, 0.0); // green
	}
	// actuators
	for (i = 0; i < 6; i++) {
		//glVertex3f(geo.b[i][0], geo.b[i][2], geo.b[i][1]);
		//glVertex3f(st.p[i][0], st.p[i][2], st.p[i][1]);
		vertices[i*2 + 12] = m_pose->b[i];
		vertices[i*2 + 12 + 1] = m_pose->p[i];
		colors[i*2 + 12] = QVector4D(1.0, 1.0, 1.0, 0.0); // white
		colors[i*2 + 12 + 1] = QVector4D(1.0, 1.0, 1.0, 0.0); // white
	}

	m_program->setAttributeArray("vertex", vertices);
	m_program->setAttributeArray("color", colors);

	m_program->enableAttributeArray("vertex");
	m_program->enableAttributeArray("color");


	// base
	f->glDrawArrays(GL_POLYGON, 0, 6);

	// platform
	f->glDrawArrays(GL_POLYGON, 6, 6);

	// actuators

	// actuators
	f->glLineWidth(3.0);
	for (i = 0; i < 6; i++) {
		f->glDrawArrays(GL_LINES, i*2 + 12, 2);
	}

	m_program->disableAttributeArray("vertex");

	m_program->release();

}

void Render::resizeGL(int width, int height) {
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	float aspect = (float)width / (float)height;

	m_projection.setToIdentity();

	m_projection.perspective(45.0f, aspect, 0.1f, 100.0f);
	//m_projection.perspective(60.0f, aspect, 0.001f, 1000.0f);

	m_view.setToIdentity();
	QVector3D eye = QVector3D(0.0f, 1.0f, 3.0f);
	QVector3D center = QVector3D(0.0f, 0.0f, 0.0f);
	QVector3D up = QVector3D(0.0f, 1.0f, 0.0f);

	m_view.lookAt(eye, center, up);

	f->glViewport(0, 0, width, height);
}
