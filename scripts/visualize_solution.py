# Plot speed / distance
# Flot weight / distance

import plotly.graph_objects as go
import plotly.express as px
from plotly.subplots import make_subplots
import csv
import argparse

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='')
    parser.add_argument(
            "-i",
            "--input",
            type=str,
            help='')
    args = parser.parse_args()

    x = []
    y_weight = []
    y_speed = []
    with open(args.input) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            x.append(float(row["Distance"]))
            y_weight.append(float(row["Weight"]))
            y_speed.append(float(row["Speed"]))

    fig = make_subplots(specs=[[{"secondary_y": True}]])
    fig.add_trace(
            go.Scatter(
                x=x,
                y=y_weight,
                line_shape="vh"),
            secondary_y=False)
    fig.add_trace(
            go.Scatter(
                x=x,
                y=y_speed,
                line_shape="vh"),
            secondary_y=True)
    fig.show()
