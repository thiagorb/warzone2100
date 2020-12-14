import cpp

from DeleteExpr d, Class WIDGET, Class DERIVED
where WIDGET.hasName("WIDGET")
and DERIVED.getABaseClass+() = WIDGET
and (
    WIDGET.refersTo(d.getDeletedObjectType())
    or
    DERIVED.refersTo(d.getDeletedObjectType())
)
select d.getDeletedObjectType(), d.getLocation()
