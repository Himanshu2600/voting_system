from reportlab.lib.pagesizes import A4
from reportlab.pdfgen import canvas
import textwrap
import sys

infile = 'data/project_analysis_report.txt'
outfile = 'data/project_analysis_report.pdf'

if len(sys.argv) >= 2:
    infile = sys.argv[1]
if len(sys.argv) >= 3:
    outfile = sys.argv[2]

with open(infile, 'r', encoding='utf-8') as f:
    text = f.read()

c = canvas.Canvas(outfile, pagesize=A4)
width, height = A4
left_margin = 50
top_margin = height - 50
max_width = width - 2 * left_margin

lines = []
for paragraph in text.split('\n'):
    if paragraph.strip() == '':
        lines.append('')
    else:
        wrapped = textwrap.wrap(paragraph, 100)
        if not wrapped:
            lines.append('')
        else:
            lines.extend(wrapped)

line_height = 12
y = top_margin
c.setFont('Helvetica', 11)
for line in lines:
    if y < 50:
        c.showPage()
        c.setFont('Helvetica', 11)
        y = top_margin
    c.drawString(left_margin, y, line)
    y -= line_height

c.save()
print('PDF generated at', outfile)
