""" Generate a dashboard based on a description.
"""

import os

import yaml
import matplotlib.pyplot as plt
import jinja2


def main():
    filename = 'demo.yml'
    with open(filename, 'r') as f:
        d = yaml.safe_load(f)
    generate_code(d)


def real_time_build_gui(description):
    print(description)
    for chart in description['charts']:
        plt.figure()
        print(chart)
        print(chart['title'])
        y = 0
        for trace in chart.get('traces', []):
            plt.plot([1, 2, 3], [y+2, y-1, y+2], label=trace['name'])
            y += 2
        plt.legend()
        plt.grid()
        plt.title(chart['title'])
    
    plt.show()


class Topic:
    def __init__(self, id, name):
        self.id = id
        self.name = name

    def __repr__(self):
        return f'Topic {self.id}: {self.name}'


class Label:
    def __init__(self, id, name, topic):
        self.id = id
        self.name = name
        self.topic = topic


def generate_code(description):
    """ Generate MQTT
    """

    labels = [
        Label(id, label['title'], label['topic'])
        for id, label in enumerate(description['labels'])
    ]

    # Figure out which topics we must subscribe to!
    topics = set(
        label['topic']
        for label in description['labels']
    )
    topics = [
        Topic(id, topic) for id, topic in enumerate(sorted(topics))
    ]
    for topic in topics:
        topic.labels = [l for l in labels if l.topic == topic.name]

    print('topics', topics)

    appname = 'dashboard1'

    backend = description['backend']
    print('backend', backend)

    data = {
        'host': backend['host'],
        'port': backend['port'],
        'appname': appname,
        'date': 'just now',
        'topics': topics,
        'labels': labels,
    }

    # Create output folder:
    output_folder = 'genapp'
    os.makedirs(output_folder, exist_ok=True)

    # Create jinja2 env:
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader('templates')
    )

    # Render main.cpp:
    template = env.get_template('main.cpp')
    with open(os.path.join(output_folder, 'main.cpp'), 'w') as f:
        f.write(template.render(data))

    # render dashboard.pro
    template = env.get_template('dashboard.pro')
    with open(os.path.join(output_folder, f'{appname}.pro'), 'w') as f:
        f.write(template.render(data))


if __name__ == "__main__":
    main()
