import numpy as np
import matplotlib.pyplot as plt
import os
import datetime


def save_graph(arg_tuple):
    filename, x_vals, y_vals_list, labels_list, color_list, some_title = arg_tuple
    import matplotlib.dates as mdates
    if type(x_vals[0]) is not datetime.date:
        x_vals = [given_date.date() for given_date in x_vals]
    x_vals = np.array(x_vals)
    plt.figure(figsize = (20, 12))
    for y_vals, col, cur_label in zip(y_vals_list, color_list, labels_list):
        mean_val = np.mean(y_vals)
        bad_indices = np.where(np.abs(y_vals) >= np.abs(mean_val) * 5 )[0]
        good_indices = np.where(np.abs(y_vals) < np.abs(mean_val) * 5 )[0]
        mean_val = np.mean(y_vals[good_indices])
        bad_indices = np.where(np.abs(y_vals) >= np.abs(mean_val) * 5 )[0]
        max_bad = -1
        if len(bad_indices) > 0:
            max_bad = np.max(bad_indices)
        plt.plot(x_vals[max_bad + 1:], y_vals[max_bad + 1:], label = cur_label, c = col)

    fig_ax = plt.gcf().get_axes()[0]
    yearsFmt = mdates.DateFormatter('%m-%d')
    fig_ax.xaxis.set_major_formatter(yearsFmt)
    days_loc = mdates.DayLocator()
    fig_ax.xaxis.set_minor_locator(days_loc)

    for ticks in fig_ax.xaxis.get_major_ticks():
            ticks.label.set_fontsize(18)

    for ticks in fig_ax.yaxis.get_major_ticks():
        ticks.label.set_fontsize(18)

    plt.legend(loc = 1, prop={'size':20}, fontsize = 'large')
    fig_ax.xaxis.set_label_text('date', fontsize=20)
    fig_ax.yaxis.set_label_text('functor_value', fontsize=20)
    plt.title(some_title, fontsize = 'xx-large')
    plt.gcf().savefig(filename, bbox_inches='tight')
    plt.close(plt.gcf())


def get_list_to_output(x_values, dict_of_lists_of_y_values, functor_names):
    from randomcolor import RandomColor
    graph_colors = RandomColor().generate(count=len(dict_of_lists_of_y_values))
    assets_names = list(dict_of_lists_of_y_values.keys())
    to_draw = [list() for _ in range(len(dict_of_lists_of_y_values) + 1)]
    for i in range(len(functor_names)):
        y_vals_list = map(lambda x: x[i][0], dict_of_lists_of_y_values.values())
        save_graph(('all_values_of_' + functor_names[i], x_values, y_vals_list, assets_names, graph_colors,
                    functor_names[i] + ' for all assets'))
        to_draw[0].append(('all_values_of_' + functor_names[i], None,
                           'Values of ' + functor_names[i] + ' for all traded assets',
                           None))

        list_of_args_for_separate_pictures = [('graph_for_' + functor_names[i] + '_for_' + cur_asset_name,
                                               x_values, [cur_y_values[i][0]], [cur_asset_name], [cur_color],
                                               functor_names[i] + ' for ' + cur_asset_name, cur_y_values[i][1])
                                              for cur_asset_name, cur_y_values, cur_color in
                                              zip(dict_of_lists_of_y_values.keys(),
                                                  dict_of_lists_of_y_values.values(),
                                                  graph_colors)]
        for ind, cur_asset_data in enumerate(list_of_args_for_separate_pictures, start=0):
            save_graph(cur_asset_data[:-1])
            to_draw[ind + 1].append((cur_asset_data[0], cur_asset_data[-1],
                                     'Values of ' + functor_names[i] + ' for ' + cur_asset_name,
                                     'Overall value of ' + functor_names[i] + ' for ' +
                                     cur_asset_name + ' is equal to '))
    return to_draw


def add_single_page(doc, values_to_add):
    from pylatex import Figure, SubFigure, NoEscape, StandAloneGraphic, VerticalSpace
    subfigures_num = len(values_to_add)/2
    subfigures = [list() for _ in range(subfigures_num)]
    for ind in range(subfigures_num):
        subfigures[ind].append(values_to_add[2*ind])
        subfigures[ind].append(values_to_add[2*ind + 1])
    if len(values_to_add) %2 == 1:
        subfigures.append([values_to_add[-1]])
    for cur_im in subfigures:
        if len(cur_im) == 2:
            for img in cur_im:
                with doc.create(Figure(position='h!')) as some_figure:
                        if img[1] is not None:
                            some_figure.append(VerticalSpace(size=NoEscape("-1cm")))
                        with doc.create(SubFigure(scale=0.27)) as cur_subfigure:
                            if img[0] is not None:
                                img_name = os.path.join(os.path.dirname(__file__), img[0] + '.png')
                                cur_subfigure.append(StandAloneGraphic(image_options=["scale=0.4"],
                                                          filename=img_name))
                            if img[2] is not None:
                                cur_subfigure.add_caption(img[2])
                        if img[1] is not None:
                            some_figure.add_caption(img[3] + str(img[1]))
        if len(cur_im) == 1:
            for img in cur_im:
                with doc.create(Figure(position='h!')) as some_figure:
                        if img[1] is not None:
                            some_figure.append(VerticalSpace(size=NoEscape("-1cm")))
                        with doc.create(SubFigure(scale=0.38)) as cur_subfigure:
                            if img[0] is not None:
                                img_name = os.path.join(os.path.dirname(__file__), img[0] + '.png')
                                cur_subfigure.append(StandAloneGraphic(image_options=["scale=0.5"],
                                                          filename=img_name))
                            #if img[2] is not None:
                            #    cur_subfigure.add_caption(img[2])
                        if img[1] is not None:
                            some_figure.add_caption(img[3] + str(img[1]))


def add_pages(doc, values_to_add, is_last = False):
    from pylatex import NewPage
    start_index = 0
    step = 4
    while start_index < len(values_to_add):
        add_single_page(doc, values_to_add[start_index : min(len(values_to_add), start_index + step)])
        if not is_last or start_index+step < len(values_to_add):
            doc.append(NewPage())
        start_index += step


def print_everything(x_values, dict_of_lists_of_y_values, functor_names, is_colored = False):
    to_output_list = get_list_to_output(x_values, dict_of_lists_of_y_values, functor_names)
    from pylatex import Document, Command
    from pylatex.package import Package
    from pylatex import PageStyle, Head, NoEscape, HorizontalSpace
    from pylatex import MiniPage,StandAloneGraphic
    geometry_options = {"top":"1.3cm", "right": "0.5cm", "left": "0.5cm"}
    doc = Document('try', documentclass='article',
                   geometry_options=geometry_options)
    if is_colored:
        doc.append(Command('pagecolor', 'LightGoldenrodYellow',
                       packages=[Package('xcolor', options = 'svgnames')]))
    header = PageStyle("header")
    with header.create(Head("L")):
        header.append(HorizontalSpace(size=NoEscape("-3.35cm")))
        with header.create(MiniPage(width=NoEscape("25.6cm"),
                                             pos='c')) as logo_wrapper:
            logo_file = os.path.join(os.path.dirname(__file__),
                                         'pdf_header_fin.png')
            logo_wrapper.append(StandAloneGraphic(image_options=["width=24.5cm", "height=1cm"],
                                                      filename=logo_file))
    doc.preamble.append(header)
    doc.change_document_style("header")

    for containments_num, cur_page_containments in enumerate(to_output_list, start=1):
        is_last_page = False
        if containments_num == len(to_output_list):
            is_last_page = True
        add_pages(doc, cur_page_containments, is_last_page)
    try:
        doc.generate_pdf(clean_tex=True, clean=True, compiler='pdflatex')
    except:
        pass

    return 4422


def generate_report(x_values, dict_of_lists_of_y_values, graph_names,
                    path, is_colored = False, balance_history=None):
    print_everything(x_values, dict_of_lists_of_y_values, graph_names, is_colored)
