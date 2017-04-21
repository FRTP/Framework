import numpy as np

from reportlab.lib.pagesizes import A4, landscape
from reportlab.graphics.shapes import Drawing
from reportlab.pdfgen import canvas
from reportlab.graphics import renderPDF
from reportlab.graphics.charts.lineplots import LinePlot

import os


# Returns given canvas with text on it.
def draw_text(input_canvas, text, x_margin, y_margin):
    input_canvas.drawString(x_margin, y_margin, text)
    return input_canvas


# Returns Drawing instance which will be written to output pdf.
def get_figure_drawing(input_data, x_margin, y_margin, width, height):
    time = np.arange(len(input_data))
    prefix_sums = np.cumsum(input_data)

    # Data in needed for Drawing format (pairs array)
    data = [np.column_stack((time, prefix_sums))]
    data = [tuple(map(tuple, data[0]))]

    # Create drawing.
    drawing = Drawing()

    # Create graph.
    our_graph = LinePlot()
    our_graph.data = data

    our_graph.x = x_margin
    our_graph.y = y_margin

    our_graph.height = height
    our_graph.width = width

    drawing.add(our_graph)

    return drawing


def generate_report(x_values, dict_of_lists_of_y_values,
                    path, balance_history=None):
    # Use current working directory if no path provided.
    if path is None:
        path = os.getcwd()

    # the_canvas = canvas.Canvas(path, pagesize=landscape(A4))

    # Draw graph to output pdf.
    # figure_drawing = get_figure_drawing(input_data, 125, 300, 600, 250)
    # renderPDF.draw(figure_drawing, the_canvas, 0, 0)

    # Writing functors values to pdf file.
    # shift_down = 0
    # for functor in functors:
    #     name = functor.get_name()
    #     value = functor.apply(input_data)
    #     the_canvas = draw_text(the_canvas, '{} : {}'.format(name, value),
    #                            50, 250 - shift_down)
    #     shift_down += 20

    # the_canvas.save()

    print("something was generated mfk")
    pass
