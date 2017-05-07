import numpy as np

# from reportlab.lib.pagesizes import A4, landscape
from reportlab.graphics.shapes import Drawing
# from reportlab.pdfgen import canvas
# from reportlab.graphics import renderPDF
from reportlab.graphics.charts.lineplots import LinePlot
import matplotlib.pyplot as plt
from fin_showings import get_default_functor_list
import os
import datetime

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

def save_graph(arg_tuple):
    filename, x_vals, y_vals_list, labels_list, color_list = arg_tuple
    import matplotlib.dates as mdates
    if type(x_vals[0]) is not datetime.date:
        x_vals = [given_date.date() for given_date in x_vals]
    x_vals = np.array(x_vals)
    #NUM_COLORS = len(color_list)
    #cm = plt.get_cmap('gist_rainbow')
    plt.figure(figsize = (20, 12))
    for y_vals, col, cur_label in zip(y_vals_list, color_list, labels_list):
        #print y_vals, col, cur_label
        mean_val = np.mean(y_vals)
        bad_indices = np.where(np.abs(y_vals) >= np.abs(mean_val) * 5 )[0]
        max_bad = -1
        if len(bad_indices) > 0:
            max_bad = np.max(bad_indices)
        plt.plot(x_vals[max_bad + 1:], y_vals[max_bad + 1:], label = cur_label, c = col)

    fig_ax = plt.gcf().get_axes()[0]
    #fig_ax.set_color_cycle([cm(1. * i / NUM_COLORS) for i in range(NUM_COLORS)])
    yearsFmt = mdates.DateFormatter('%m-%d')
    fig_ax.xaxis.set_major_formatter(yearsFmt)
    days_loc = mdates.DayLocator()  # every month
    fig_ax.xaxis.set_minor_locator(days_loc)

    plt.legend(loc = 1)
    plt.xlabel('date')
    plt.ylabel('functor_value')
    plt.gcf().savefig(filename, bbox_inches='tight')
    plt.close(plt.gcf())

def get_list_to_output(x_values, dict_of_lists_of_y_values, functor_names):
    #print dict_of_lists_of_y_values
    from randomcolor import RandomColor
    graph_colors = RandomColor().generate(count=len(dict_of_lists_of_y_values))
    assets_names = list(dict_of_lists_of_y_values.keys())
    to_draw = [list() for _ in range(len(dict_of_lists_of_y_values) + 1)]
    for i in range(len(functor_names)):
        y_vals_list = map(lambda x: x[i][0], dict_of_lists_of_y_values.values())
        #print len(x_values), len(y_vals_list[0])
        save_graph(('all_values of ' + functor_names[i], x_values, y_vals_list, assets_names, graph_colors))
        to_draw[0].append(('all_values of ' + functor_names[i], None))

        list_of_args_for_separate_pictures = [('graph for ' + functor_names[i] + ' for ' + cur_asset_name,
                                               x_values, [cur_y_values[i][0]], [cur_asset_name], [cur_color],
                                               cur_y_values[i][1])
                                              for cur_asset_name, cur_y_values, cur_color in
                                              zip(dict_of_lists_of_y_values.keys(),
                                                  dict_of_lists_of_y_values.values(),
                                                  graph_colors)]
        for ind, cur_asset_data in enumerate(list_of_args_for_separate_pictures):
            #print cur_asset_data[:-1]
            save_graph(cur_asset_data[:-1])
            #print 'kuku'
            to_draw[ind + 1].append((cur_asset_data[0], cur_asset_data[-1]))
    return to_draw



def main(fname, width, *args, **kwargs):
    geometry_options = {"right": "2cm", "left": "2cm"}
    doc = Document(fname, geometry_options=geometry_options)

    doc.append('Introduction.')

    with doc.create(Section('I am a section')):
        doc.append('Take a look at this beautiful plot:')

        with doc.create(Figure(position='htbp')) as plot:
            plot.add_plot(width=NoEscape(width), *args, **kwargs)
            plot.add_caption('I am a caption.')

        doc.append('Created using matplotlib.')

    doc.append('Conclusion.')

    doc.generate_pdf(clean_tex=False)

def print_everything(x_values, dict_of_lists_of_y_values, functor_names):
    to_output_list = get_list_to_output(x_values, dict_of_lists_of_y_values, functor_names)
    return 4422

'''def generate_report(x_values, dict_of_lists_of_y_values,
                    path, balance_history=None, graph_names):
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
    pass'''

def generate_report(x_values, dict_of_lists_of_y_values, graph_names,
                    path, balance_history=None):
    print_everything(x_values, dict_of_lists_of_y_values, graph_names)

def some_rand_value(some_length):
    start_price = list(np.random.randint(low = 10, high = 200, size = 1) * 1.0 /40)[0]
    start_num_of_assets = 0
    prices = [start_price]
    assets_num = [start_num_of_assets]
    for i in range(1, some_length):
        cur_prices_step = np.random.normal(loc= 3, scale=1.0, size = 1)
        cur_assets_step = np.random.randint(low = -40, high = 80, size = 1)
        prices.append(prices[-1] + cur_prices_step)
        assets_num.append(assets_num[-1] + cur_assets_step)
    prices = np.array(prices)
    assets_num = np.array(assets_num)
    return np.stack((prices, assets_num))

def some_random_data():
    import datetime
    import matplotlib.dates as mdates
    numdays = 100
    base = datetime.datetime.today()
    date_list = [base - datetime.timedelta(days=x) for x in range(1, numdays)]
    date_list = np.sort(np.array(date_list))
    #print map(lambda x: str(x), date_list)
    date_list = np.array([given_date.date() for given_date in date_list])
    assets_names = ['AAPL', 'JAZHKA']
    prices_and_counts = {}
    graph_data = {}
    functors = get_default_functor_list()
    functor_names = [functor.get_name() for functor in functors]
    for asset in assets_names:
        prices_and_counts[asset] = some_rand_value(len(date_list) + 1)
        prices_and_counts_of_one_asset = prices_and_counts[asset]
        value = [(functor.apply_graph(prices_and_counts_of_one_asset, 200000),
                  functor.apply_value(prices_and_counts_of_one_asset, 200000))
                 for functor in functors]
        graph_data[asset] = value

    generate_report(x_values=date_list, path=None,
                    dict_of_lists_of_y_values=graph_data,
                    balance_history=None,
                    graph_names=functor_names)
    '''print date_list
    plt.plot(date_list, [1] * date_list.shape[0])
    ax_list = plt.gcf().get_axes()[0]

    years = mdates.YearLocator()  # every year
    yearsFmt = mdates.DateFormatter('%m-%d')
    #ax_list.xaxis.set_major_locator(years)
    ax_list.xaxis.set_major_formatter(yearsFmt)
    months = mdates.DayLocator()  # every month
    ax_list.xaxis.set_minor_locator(months)
    plt.show()'''



some_random_data()
