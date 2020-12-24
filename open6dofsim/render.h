#pragma once

#include <qopenglwidget>
#include <qmatrix4x4.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include "geo6dof.h"

class Render : public QOpenGLWidget
{
	Q_OBJECT

public:
	explicit Render(QWidget* parent = 0);
	~Render();
	void updateDisplay(struct Geo6Dof::display_pose *pose);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

private:
	QMatrix4x4 m_projection;
	QMatrix4x4 m_view;
	QOpenGLShaderProgram *m_program;
	struct Geo6Dof::display_pose* m_pose = NULL;
};