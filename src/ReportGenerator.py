#!/usr/bin/python

import numpy as np

from reportlab.lib.pagesizes import A4, landscape
from reportlab.graphics.shapes import Drawing
from reportlab.pdfgen import canvas
from reportlab.graphics import renderPDF
from reportlab.graphics.charts.lineplots import LinePlot


def draw_text(input_canvas, text, x_margin, y_margin):
    input_canvas.drawString(x_margin, y_margin, text)
    return input_canvas


def get_figure_drawing(input_data, x_margin, y_margin, width, height):
    time = np.arange(len(input_data))
    prefix_sums = np.cumsum(input_data)

    # data in needed for Drawing format (pairs array)
    data = [np.column_stack((time, prefix_sums))]
    data = [tuple(map(tuple, data[0]))]

    drawing = Drawing(400, 200)

    our_graph = LinePlot()
    our_graph.data = data

    # ok
    our_graph.x = x_margin
    our_graph.y = y_margin
    # ok
    our_graph.height = height
    our_graph.width = width

    drawing.add(our_graph)

    return drawing


def generate_report(input_data, functors, path):
    the_canvas = canvas.Canvas(path, pagesize=landscape(A4))

    # draw graph
    figure_drawing = get_figure_drawing(input_data, 125, 300, 600, 250)
    renderPDF.draw(figure_drawing, the_canvas, 0, 0)

    # write functor values
    shift_down = 0
    for functor in functors:
        the_canvas = draw_text(the_canvas, '{} : {}'.format(functor.get_name(), functor.apply(input_data)),
                               50, 250 - shift_down)
        shift_down += 20

    the_canvas.save()
